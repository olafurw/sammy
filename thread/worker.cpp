#include "worker.hpp"

#include "pool.hpp"
#include "task.hpp"

#include "../server.hpp"

namespace sammy
{
namespace thread
{

worker::worker(unsigned int index)
{
    m_index = index;
}

void worker::run()
{
    m_thread = std::thread([&](){
        while( true )
        {
            std::shared_ptr<task> next_task = nullptr;

            {
                std::lock_guard<std::mutex> lock(m_mutex);

                if(!m_tasks.empty())
                {
                    next_task = m_tasks.front();
                    m_tasks.pop();
                }
            }

            if(next_task == nullptr)
            {
                // Chill
                std::this_thread::sleep_for( std::chrono::milliseconds( 25 ) );

                continue;
            }

            sammy::server server{ next_task };
            server.handle();
        }
    });
}

void worker::add_task(std::shared_ptr<task> task)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_tasks.push(task);
}

}
}

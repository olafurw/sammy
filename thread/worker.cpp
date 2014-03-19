#include "worker.hpp"

#include "pool.hpp"
#include "task.hpp"

#include "../server.hpp"

namespace sammy
{
namespace thread
{

worker::worker(sammy::thread::pool* tp)
{
    m_thread = std::thread(&worker::work, this, tp);
}

void worker::work(sammy::thread::pool* tp)
{
    while( true )
    {
        // We have stuff to do (if we are quick enough)
        if(tp->has_task())
        {
            // Between the has_task and get_task, someone got it, go again
            auto task = tp->get_task();
            if(task == nullptr)
            {
                continue;
            }

            // Serve the content
            sammy::server server{ task };
            server.handle();
        }

        // Chill
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    }
}

}
}

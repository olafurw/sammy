#include "pool.hpp"

#include "../domain.hpp"
#include "../server.hpp"
#include "../request.hpp"

#include <thread>
#include <chrono>

namespace sammy
{
namespace thread
{

pool::pool()
{
    for(unsigned int i = 0; i < 10; ++i)
    {
        m_workers.emplace_back(worker(this));
    }
}

void pool::add_task(std::shared_ptr<task> t)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_task_queue.push(t);
}

std::shared_ptr<task> pool::get_task()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    // Nothing to do
    if(m_task_queue.empty())
    {
        return nullptr;
    }

    auto task = m_task_queue.front();
    m_task_queue.pop();

    return task;
}

bool pool::has_task() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return !m_task_queue.empty();
}

}
}

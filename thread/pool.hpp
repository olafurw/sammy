#ifndef _SAMMY_THREAD_POOL_
#define _SAMMY_THREAD_POOL_

#include <memory>
#include <queue>
#include <mutex>

#include "worker.hpp"
#include "task.hpp"

namespace sammy
{
namespace thread
{

class pool
{
public:
    pool();

    void add_task(std::shared_ptr<task> t);

    std::shared_ptr<task> get_task();

    bool has_task() const;

private:
    std::vector<worker> m_workers;
    std::queue<std::shared_ptr<task>> m_task_queue;
    mutable std::mutex m_mutex;
};

}
}

#endif

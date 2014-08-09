#ifndef _SAMMY_THREAD_WORKER_
#define _SAMMY_THREAD_WORKER_

#include <thread>
#include <queue>
#include <mutex>

namespace sammy
{
namespace thread
{

class task;
class worker
{
public:
    worker(unsigned int index);
    void run();
    void add_task(std::shared_ptr<task> task);

private:
    unsigned int m_index;    
    std::thread m_thread;
    std::queue<std::shared_ptr<task>> m_tasks;

    mutable std::mutex m_mutex;
};

}
}

#endif

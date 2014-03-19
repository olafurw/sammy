#ifndef _SAMMY_THREAD_WORKER_
#define _SAMMY_THREAD_WORKER_

#include <thread>

namespace sammy
{
namespace thread
{

class pool;
class worker
{
public:
    worker(sammy::thread::pool* tp);

private:
    void work(sammy::thread::pool* tp);

    std::thread m_thread;
};

}
}

#endif

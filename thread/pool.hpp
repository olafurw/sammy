#ifndef _SAMMY_THREAD_POOL_
#define _SAMMY_THREAD_POOL_

#include <memory>

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

private:
    std::vector<std::shared_ptr<worker>> m_workers;
};

}
}

#endif

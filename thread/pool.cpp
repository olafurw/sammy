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
        auto w = std::make_shared<worker>(i);
        w->run();
        m_workers.push_back(w);
    }
}

void pool::add_task(std::shared_ptr<task> t)
{
    int worker_id = sammy::utils::random(0, 9);

    m_workers[worker_id]->add_task(t);
}

}
}

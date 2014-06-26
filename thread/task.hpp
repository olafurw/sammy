#ifndef _SAMMY_THREAD_TASK_
#define _SAMMY_THREAD_TASK_

#include <memory>

#include "../domain.hpp"
#include "../request.hpp"
#include "../cache_storage.hpp"

namespace sammy
{
namespace thread
{

class task
{
public:
    int newsockfd;
    std::shared_ptr<sammy::domain> domain;
    std::shared_ptr<sammy::request> client_request;
    std::shared_ptr<sammy::cache_storage> cache;
    std::string client_address;

    task(int sock,
         std::shared_ptr<sammy::domain> d,
         std::shared_ptr<sammy::request> r,
         std::shared_ptr<sammy::cache_storage> cs,
         std::string ca):
             newsockfd(sock),
             domain(d),
             client_request(r),
             cache(cs),
             client_address(ca)
    {

    }
};

}
}

#endif

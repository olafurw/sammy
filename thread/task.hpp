#ifndef _SAMMY_THREAD_TASK_
#define _SAMMY_THREAD_TASK_

#include <memory>

#include "../domain.hpp"
#include "../request.hpp"

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
    std::string client_address;

    task(int sock,
         std::shared_ptr<sammy::domain> d,
         std::shared_ptr<sammy::request> r,
         std::string ca):
             newsockfd(sock),
             domain(d),
             client_request(r),
             client_address(ca)
    {

    }
};

}
}

#endif

#ifndef _SAMMY_SERVER_
#define _SAMMY_SERVER_

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <net/route.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#include <thread>
#include <memory>

#include "log.hpp"
#include "utils/utils.hpp"
#include "response.hpp"
#include "request.hpp"
#include "config.hpp"

#include "thread/task.hpp"

namespace sammy
{
class server
{
public:
    server(std::shared_ptr<sammy::thread::task> task);

    void handle();
    ~server();

private:
    void accept_request();
    void read_request(size_t& read_result, std::string& buffer_str);
    std::string static_file_response();
    std::string plain_file_response(const sammy::path& path);
    std::string python_response(const sammy::path& path, std::string post_data = "");
    std::string binary_response(const sammy::path& path, std::string post_data = "");
    std::string file_not_found_response();

    int m_sockfd;

    std::shared_ptr<sammy::domain> m_domain;
    std::shared_ptr<sammy::request> m_request;
    std::shared_ptr<sammy::cache_storage> m_cache;
    std::unique_ptr<sammy::log> m_log;
    std::string m_client_address;
};
}

#endif

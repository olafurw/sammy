#ifndef _WOT_SERVER_
#define _WOT_SERVER_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/route.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <thread>
#include <memory>

#include "log.hpp"
#include "utils/utils.hpp"
#include "response.hpp"
#include "request.hpp"
#include "config.hpp"

namespace wot
{
class server
{
public:
    server(std::shared_ptr<wot::domain> domain,
           std::shared_ptr<wot::request> request, 
           std::string client_address, 
           int sockfd);

    void handle();
    ~server();

private:
    void accept_request();
    void read_request(size_t& read_result, std::string& buffer_str);
    std::string static_file_response();
    std::string plain_file_response(const wot::path& path);
    std::string python_response(const wot::path& path, std::string post_data = "");
    std::string binary_response(const wot::path& path, std::string post_data = "");
    std::string file_not_found_response();

    int m_sockfd;

    std::shared_ptr<wot::domain> m_domain;
    std::shared_ptr<wot::request> m_request;
    std::unique_ptr<wot::log> m_log;
    std::string m_client_address;
};
}

#endif

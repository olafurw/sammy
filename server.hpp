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

#include <memory>

#include "log.hpp"
#include "utils.hpp"
#include "response.hpp"
#include "request.hpp"
#include "config.hpp"

namespace wot
{
class server
{
public:
    server();
    void handle();
    ~server();

private:
    void accept_request();
    void read_request(size_t& read_result, std::string& buffer_str);
    std::string static_file_response(std::shared_ptr<domain> domain, const wot::request& request);
    std::string plain_file_response(std::shared_ptr<domain> domain, const wot::path& path);
    std::string python_response(std::shared_ptr<domain> domain, const wot::path& path);
    std::string binary_response(std::shared_ptr<domain> domain, const wot::path& path, const wot::request& request);
    std::string file_not_found_response(std::shared_ptr<domain> domain);

    int m_portno;
    int m_sockfd;
    int m_newsockfd;

    socklen_t m_clilen;
    sockaddr_in m_serv_addr;
    sockaddr_in m_cli_addr;

    std::unique_ptr<wot::log> m_log;
    std::unique_ptr<wot::domains> m_domains;
};
}

#endif

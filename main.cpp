#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <locale>
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>

#include "utils/utils.hpp"
#include "thread/pool.hpp"
#include "cache_storage.hpp"
#include "request.hpp"
#include "server.hpp"

static const unsigned int READ_SIZE = 8192;
static const char* PORT_NUMBER = "80";
static const unsigned int CONNECTION_QUEUE_SIZE = 30;
static const unsigned int MAX_EPOLL_EVENTS = 30;

void read_request(int sockfd, size_t& read_result, std::string& buffer_str)
{
    // Create and zero out the buffer
    char buffer[READ_SIZE];
    memset(buffer, 0, READ_SIZE);

    read_result = read(sockfd, buffer, READ_SIZE - 1);
    buffer_str = buffer;
}

int fd_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1)
    {
        return -1;
    }

    flags |= O_NONBLOCK;

    const int result = fcntl(fd, F_SETFL, flags);
    if(result == -1)
    {
        return -1;
    }

    return 0;
}

addrinfo* create_addrinfo()
{
    addrinfo* res{ nullptr };

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    const auto info_result = getaddrinfo(NULL, PORT_NUMBER, &hints, &res);
    if(info_result < 0)
    {
        std::cout << "Error in address info, error #" << info_result << "" << std::endl;
        exit(1);
    }
    
    return res;
}

int create_socket(addrinfo* res)
{
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1)
    {
        std::cout << "Error creating socket, error #" << errno << "" << std::endl;
        exit(1);
    }
    
    return sockfd;
}

void set_socket_reusable(int socket)
{
    const int yes = 1;
    const auto sockopt_result = setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if(sockopt_result == -1)
    {
        std::cout << "Error setting socket options, error #" << errno << std::endl;
        exit(1);
    }
}

void set_socket_bind(int socket, addrinfo* res)
{
    const auto bind_result = bind(socket, res->ai_addr, res->ai_addrlen);
    if(bind_result == -1)
    {
        std::cout << "Error binding address, error #" << errno << "" << std::endl;
        exit(1);
    }
}

void set_socket_non_blocking(int socket)
{
    const auto non_blocking_result = fd_nonblock(socket);
    if(non_blocking_result == -1)
    {
        std::cout << "Error setting socket to non blocking. error #" << errno << std::endl;
        exit(1);
    }
}

void set_socket_listen(int socket)
{
    const auto listen_result = listen(socket, CONNECTION_QUEUE_SIZE);
    if(listen_result == -1)
    {
        std::cout << "Error listening to address, error #" << errno << "" << std::endl;
        exit(1);
    }   
}

int create_epoll()
{
    int efd = epoll_create1(0);
    if(efd == -1)
    {
        std::cout << "Error creating epoll file descriptor, error #" << errno << "" << std::endl;
        exit(1);
    }
    
    return efd;
}

void set_epoll_interface(int efd, int socket)
{
    epoll_event event;
    event.data.fd = socket;
    event.events = EPOLLIN | EPOLLET;
    
    const auto ctl_result = epoll_ctl(efd, EPOLL_CTL_ADD, socket, &event);
    if(ctl_result == -1)
    {
        std::cout << "Error creating epoll control interface, error #" << errno << "" << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    auto log = std::unique_ptr<sammy::log>(new sammy::log());

    addrinfo* res = create_addrinfo();
    int sockfd = create_socket(res);
    
    set_socket_reusable(sockfd);
    set_socket_bind(sockfd, res);
    set_socket_non_blocking(sockfd);
    set_socket_listen(sockfd);
    
    int efd = create_epoll();

    set_epoll_interface(efd, sockfd);

    std::cout << "Starting server." << std::endl;
    
    // Init the domain config loading
    auto domain_storage = std::make_shared<sammy::domain_storage>();
    if(domain_storage->errors())
    {
        std::cout << "Error loading config!" << std::endl;
        exit(1);
    }

    // Init cache
    auto cache = std::make_shared<sammy::cache_storage>();

    // Init events
    epoll_event* events = new epoll_event[MAX_EPOLL_EVENTS];

    sammy::thread::pool thread_pool;
    
    std::map<int, std::string> client_ip_address;

    std::cout << "Server started!" << std::endl;

    while( true )
    {
        const int event_count = epoll_wait(efd, events, MAX_EPOLL_EVENTS, -1);

        for(int i = 0; i < event_count; ++i)
        {
            const epoll_event& event = events[i];
            
            if( (event.events & EPOLLERR) ||
                (event.events & EPOLLHUP) ||
                (!(event.events & EPOLLIN)))
            {
                std::cout << "Error in the file descriptor." << std::endl;
                close(event.data.fd);
                continue;
            }
            else if(sockfd == event.data.fd)
            {
                while( true )
                {
                    sockaddr_in cli_addr;
                    socklen_t clilen{ sizeof(cli_addr) };

                    // Accept the the request
                    int newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen);
                    if(newsockfd == -1)
                    {
                        break;
                    }

                    set_socket_non_blocking(sockfd);
                    set_epoll_interface(efd, newsockfd);
                    
                    char out_c[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &(cli_addr.sin_addr), out_c, INET_ADDRSTRLEN);
                    client_ip_address[newsockfd] = out_c;
                }
                
                continue;
            }
            else
            {
                int newsockfd = event.data.fd;
                
                std::string client_address = client_ip_address[newsockfd];
                
                std::string request_str = "";
                size_t read_result = 0;
                read_request(newsockfd, read_result, request_str);

                // If the read result isn't ok
                if(read_result == -1)
                {
                    log->error("Error in read result, error #" + std::to_string(errno));
                    close(newsockfd);
                    continue;
                }

                // The request string is empty
                if(request_str.empty())
                {
                    log->error("Request string empty.");
                    close(newsockfd);
                    continue;
                }

                // Parse request string
                auto client_request = std::make_shared<sammy::request>(request_str);
                if(client_request->errors())
                {
                    log->error("Request parse error: " + client_request->error_text());
                    close(newsockfd);
                    continue;
                }
                
                // Load domain configuration
                std::string response = "";
                auto domain = domain_storage->get_domain(client_request->get_host());

                // If this server does not serve the domain being asked for, we close the connection.
                if(!domain)
                {
                    log->error(client_address + " : " + client_request->get_host() + " : " + client_request->get_method() + " : " + client_request->get_path() + " : Domain not served.");
                    close(newsockfd);
                    continue;
                }

                // If the client address is blacklisted, we close the connection.
                if(domain->is_blacklisted(client_address))
                {
                    log->error(client_address + " : " + client_request->get_host() + " : " + client_request->get_method() + " : " + client_request->get_path() + " : IP blacklisted.");
                    close(newsockfd);
                    continue;
                }

                // Add the task, to be processed by the thread pool when possible
                thread_pool.add_task(std::make_shared<sammy::thread::task>(newsockfd, domain, client_request, cache, client_address));
                
                continue;
            }
        }
    }

    delete [] events;

    return 0;
}

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
#include "request.hpp"
#include "server.hpp"

void read_request(int sockfd, size_t& read_result, std::string& buffer_str)
{
    const unsigned int read_size = 8192;

    // Create and zero out the buffer
    char buffer[read_size];
    memset(buffer, 0, read_size);

    read_result = read(sockfd, buffer, read_size - 1);
    buffer_str = buffer;
}

int set_non_blocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1)
    {
        return -1;
    }

    flags |= O_NONBLOCK;

    int result = fcntl(fd, F_SETFL, flags);
    if(result == -1)
    {
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    auto log = std::unique_ptr<sammy::log>(new sammy::log());

    addrinfo* res{ nullptr };

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    auto info_result = getaddrinfo(NULL, "80", &hints, &res);
    if(info_result < 0)
    {
        std::cout << "Error in address info, error #" << info_result << "" << std::endl;
    }

    // Create the socket
    auto sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1)
    {
        std::cout << "Error creating socket, error #" << errno << "" << std::endl;
        exit(1);
    }

    // Bind the socket with the address information given above
    auto bind_result = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if(bind_result == -1)
    {
        std::cout << "Error binding address, error #" << errno << "" << std::endl;
        exit(1);
    }

    // Set the socket as non blocking
    auto non_blocking_result = set_non_blocking(sockfd);
    if(non_blocking_result == -1)
    {
        std::cout << "Error setting socket to non blocking. error #" << errno << std::endl;
        exit(1);
    }

    // Listen on the socket, with possible 20 connections that can wait in the backlog
    auto listen_result = listen(sockfd, 20);
    if(listen_result == -1)
    {
        std::cout << "Error listening to address, error #" << errno << "" << std::endl;
        exit(1);
    }

    // Creating the epoll file descriptor
    auto efd = epoll_create1(0);
    if(efd == -1)
    {
        std::cout << "Error creating epoll file descriptor." << std::endl;
        exit(1);
    }

    // Create the epoll control interface
    epoll_event event;
    event.data.fd = sockfd;
    event.events = EPOLLIN | EPOLLET;
    auto ctl_result = epoll_ctl(efd, EPOLL_CTL_ADD, sockfd, &event);
    if(ctl_result == -1)
    {
        std::cout << "Error creating epoll control interface." << std::endl;
        exit(1);
    }

    std::cout << "Starting server." << std::endl;
    
    // Init the domain config loading
    auto domains = std::make_shared<sammy::domains>();
    if(domains->errors())
    {
        std::cout << "Error loading config!" << std::endl;
        exit(1);
    }

    const int MAX_EVENTS = 20;
    epoll_event* events = new epoll_event[MAX_EVENTS];

    sammy::thread::pool thread_pool;

    std::cout << "Server started!" << std::endl;

    while( true )
    {
        auto event_count = epoll_wait(efd, events, MAX_EVENTS, -1);

        for(int i = 0; i < event_count; ++i)
        {
            if( (events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                std::cout << "Error in the file descriptor." << std::endl;
                close(events[i].data.fd);
                continue;
            }
            else if(sockfd == events[i].data.fd)
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

                    auto non_blocking_result = set_non_blocking(sockfd);
                    if(non_blocking_result == -1)
                    {
                        std::cout << "Error setting socket to non blocking. error #" << errno << std::endl;
                        exit(1);
                    }

                    epoll_event newevent;
                    newevent.data.fd = newsockfd;
                    newevent.events = EPOLLIN | EPOLLET;
                    auto ctl_result = epoll_ctl(efd, EPOLL_CTL_ADD, newsockfd, &newevent);
                    if(ctl_result == -1)
                    {
                        std::cout << "Error creating epoll control interface." << std::endl;
                        exit(1);
                    }
                }
                
                continue;
            }
            else
            {
                // Log who is requesting the data
                //char inet_str[INET_ADDRSTRLEN];
                //inet_ntop(cli_addr.sin_family, &(cli_addr.sin_addr), inet_str, INET_ADDRSTRLEN);

                std::string client_address("127.0.0.1");

                auto newsockfd = events[i].data.fd;
                
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
                auto domain = domains->get_domain(client_request->get_host());

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
                thread_pool.add_task(std::make_shared<sammy::thread::task>(newsockfd, domain, client_request, client_address));
                
                continue;
            }
        }
    }

    delete [] events;

 
    return 0;
}

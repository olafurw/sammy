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

    auto sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if(sockfd == -1)
    {
        std::cout << "Error creating socket, error #" << errno << "" << std::endl;
        exit(1);
    }

    auto bind_result = bind(sockfd, res->ai_addr, res->ai_addrlen);

    // Bind the socket with the address information given above
    if(bind_result == -1)
    {
        std::cout << "Error binding address, error #" << errno << "" << std::endl;
        exit(1);
    }

    auto listen_result = listen(sockfd, 20);

    // Listen on the socket, with possible 20 connections that can wait in the backlog
    if(listen_result == -1)
    {
        std::cout << "Error listening to address, error #" << errno << "" << std::endl;
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

    sammy::thread::pool thread_pool;

    std::cout << "Server started!" << std::endl;
   
    while( true )
    {
        sockaddr_in cli_addr;
        socklen_t clilen{ sizeof(cli_addr) };

        // Accept the the request
        int newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen);

        if(!newsockfd)
        {
            log->error("Error in client socket connection. Skipping!");
            continue;
        }

        // Log who is requesting the data
        char inet_str[INET_ADDRSTRLEN];
        inet_ntop(cli_addr.sin_family, &(cli_addr.sin_addr), inet_str, INET_ADDRSTRLEN);

        std::string client_address(inet_str);
        
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
    }
 
    return 0;
}

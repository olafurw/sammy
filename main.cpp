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

    char p;
    size_t peek_size = recv(sockfd, &p, 1, MSG_PEEK);
    if(peek_size < 1)
    {
        read_result = peek_size;
        return;
    }

    read_result = read(sockfd, buffer, read_size - 1);
    buffer_str = buffer;
}

int main(int argc, char *argv[])
{
    auto log = std::unique_ptr<sammy::log>(new sammy::log());

    // Create the socket and set the socket options
    int portno{ 80 };
    int sockfd{ socket(AF_INET, SOCK_STREAM, 0) };
    int optval{ 1 };
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Set the current server address information
    sockaddr_in serv_addr;
    memset((char*)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket with the address information given above
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));

    // Listen on the socket, with possible 20 connections that can wait in the backlog
    listen(sockfd, 20);

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
            close(newsockfd);
            continue;
        }

        // The request string is empty
        if(request_str.empty())
        {
            close(newsockfd);
            continue;
        }

        // Parse request string
        auto client_request = std::make_shared<sammy::request>(request_str);
        if(client_request->errors())
        {
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

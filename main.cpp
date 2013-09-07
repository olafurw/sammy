#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <locale>
#include <functional>
#include <algorithm>
#include <thread>
#include <mutex>

#include "utils.hpp"
#include "log.hpp"
#include "request.hpp"
#include "server.hpp"

int main(int argc, char *argv[])
{
    // Init the logger
    auto log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    log->write(wot::log::type::info) << "Starting Server." << std::endl;
    
    // Create the socket and set the socket options
    int portno{ 80 };
    int sockfd{ socket(AF_INET, SOCK_STREAM, 0) };
    int optval{ 1 };
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Set the current server address information
    sockaddr_in serv_addr;
    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // Bind the socket with the address information given above
    bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    log->write(wot::log::type::info) << "Socket Bound." << std::endl;

    // Listen on the socket, with possible 20 connections that can wait in the backlog
    listen(sockfd, 20);
    log->write(wot::log::type::info) << "Listening to socket." << std::endl;

    log->write(wot::log::type::info) << "Server Started." << std::endl;
    
    // Init the domain config loading
    auto domains = std::make_shared<wot::domains>();
    if(domains->errors())
    {
        log->write(wot::log::type::error) << "Error loading allowed domains config." << std::endl;
        exit(1);
    }

    log->write(wot::log::type::info) << "Allowed domains config loaded." << std::endl;

    std::mutex process_mutex;
    std::vector<std::thread::id> process_ids;
   
    while( true )
    {
        // Accept socket request
        log->write(wot::log::type::info) << "Waiting for a request." << std::endl;
        log->write(wot::log::type::info) << "Currently: " << process_ids.size() << " processes alive." << std::endl;
    
        sockaddr_in cli_addr;
        socklen_t clilen{ sizeof(cli_addr) };

        // Accept the the request
        int newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen);

        // Log who is requesting the data
        char inet_str[INET_ADDRSTRLEN];
        inet_ntop(cli_addr.sin_family, &(cli_addr.sin_addr), inet_str, INET_ADDRSTRLEN);
        log->write(wot::log::type::info) << "Request accepted from: " << inet_str << std::endl;

        std::thread thread { [ newsockfd, domains, &process_mutex, &process_ids ]() {
                                process_mutex.lock();
                                process_ids.push_back(std::this_thread::get_id());
                                process_mutex.unlock();

                                wot::server server{ newsockfd, domains };
                                server.handle();
                                
                                process_mutex.lock();
                                process_ids.erase(std::remove(process_ids.begin(), process_ids.end(), std::this_thread::get_id()), process_ids.end());
                                process_mutex.unlock();
                              } 
                           };

        thread.detach();
    }
 
    return 0;
}

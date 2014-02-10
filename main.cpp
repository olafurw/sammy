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
#include "request.hpp"
#include "server.hpp"

int main(int argc, char *argv[])
{
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

    // Listen on the socket, with possible 20 connections that can wait in the backlog
    listen(sockfd, 20);
    
    std::mutex process_mutex;
    std::vector<std::thread::id> process_ids;
   
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

        // Spawn the handling thread and detach it, let it finish on its own
        std::thread thread { [ newsockfd, &process_mutex, &process_ids, client_address ]() {
                                // Store the thread id in the process list
                                process_mutex.lock();
                                process_ids.push_back(std::this_thread::get_id());
                                process_mutex.unlock();

                                wot::server server{ client_address, newsockfd };
                                server.handle();

                                // Remove the thread id from the process list, since we are done handling the request
                                process_mutex.lock();
                                process_ids.erase(std::remove(process_ids.begin(), process_ids.end(), std::this_thread::get_id()), process_ids.end());
                                process_mutex.unlock();
                              } 
                           };

        thread.detach();
    }
 
    return 0;
}

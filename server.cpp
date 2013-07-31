/* A simple server in the internet domain using TCP
 *    The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <net/route.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <locale>
#include <functional>
#include <algorithm>

#include "utils.hpp"
#include "log.hpp"
#include "request.hpp"

std::string Response(const std::string& message, const std::string& type)
{
    std::stringstream ss;
    ss << "HTTP/1.0 200 OK\n";
    ss << "Server: tiaswot\n";
    ss << "MIME-version: 1.0\n";
    ss << "Content-type: " << type << "\n";
    ss << "Last-Modified: Thu Jul 7 00:25:33 1994\n";
    ss << "Content-Length: " << message.size() << "\n\n";
    ss << message << "\n\n";
   
    return ss.str(); 
}

class Server
{
public:
    Server()
    {
        log = std::unique_ptr<wot::log>(new wot::log(std::cout));
        log->write(wot::log::type::info) << "Starting Server." << std::endl;

        portno = 80;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        int optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        bzero((char*)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
        log->write(wot::log::type::info) << "Socket Bound." << std::endl;

        listen(sockfd, 5);
        log->write(wot::log::type::info) << "Listening to socket." << std::endl;

        clilen = sizeof(cli_addr);

        log->write(wot::log::type::info) << "Server Started." << std::endl;
    }

    void handle()
    {
        try
        {
            log->write(wot::log::type::info) << "Waiting for a request." << std::endl;

            buffer_size = 1024;
            buffer = new char[buffer_size];
            bzero(buffer, buffer_size);

            newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen);
            bzero(buffer, buffer_size);
            
            log->write(wot::log::type::info) << "-------------" << std::endl;

            char inet_str[INET_ADDRSTRLEN];
            inet_ntop(cli_addr.sin_family, &(cli_addr.sin_addr), inet_str, INET_ADDRSTRLEN);
            log->write(wot::log::type::info) << "Request accepted from: " << inet_str << std::endl;

            read(newsockfd, buffer, buffer_size - 1);

            std::string bufferStr(buffer);

            log->write(wot::log::type::info) << "Packet: " << std::endl << bufferStr << std::endl;

            wot::request client_request(bufferStr);

            if(client_request.errors() == false)
            {
                std::string response;

                if( (client_request.get_host() == "www.cznp.com" || client_request.get_host() == "cznp.com") && client_request.get_method() == "GET" && client_request.get_path() == "/")
                {
                    response = Response(wot::utils::file_to_string("/home/cznp/index.html"), "text/html");
                }
                else
                {
                    response = Response(wot::utils::file_to_string("/home/cznp/404.html"), "text/html");
                }

                write(newsockfd, response.c_str(), response.size());
            }

            close(newsockfd);
            delete [] buffer;
        }
        catch(...)
        {
            std::cout << buffer << std::endl;
            close(newsockfd);
            delete [] buffer;
            return;
        }
    }

    ~Server()
    {
        close(newsockfd);
        close(sockfd);

        delete [] buffer;
    }

private:
    int portno;
    int sockfd;
    int newsockfd;

    socklen_t clilen;
    sockaddr_in serv_addr;
    sockaddr_in cli_addr;

    char* buffer;
    size_t buffer_size;

    std::unique_ptr<wot::log> log;
};

int main(int argc, char *argv[])
{
    Server server;
    while(true)
    {
        server.handle();
    }
    return 0;
}

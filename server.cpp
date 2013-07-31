/* A simple server in the internet domain using TCP
 *    The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

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
        wot::log::info("Starting Server.");
        portno = 80;
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        int optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        bzero((char*)&serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);

        bind(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
        wot::log::info("Socket Bound");

        listen(sockfd, 5);
        wot::log::info("Listening to socket");

        clilen = sizeof(cli_addr);

        wot::log::info("Server Started.");
    }

    void handle()
    {
        try
        {
            wot::log::info("Waiting for a request.");

            buffer_size = 1024;
            buffer = new char[buffer_size];
            bzero(buffer, buffer_size);

            newsockfd = accept(sockfd, (sockaddr*)&cli_addr, &clilen);
            bzero(buffer, buffer_size);
            
            wot::log::info("-----------------");
            wot::log::info("Request accepted.");

            read(newsockfd, buffer, buffer_size - 1);

            std::string bufferStr(buffer);

            wot::log::info("Packet:\n" + bufferStr);

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

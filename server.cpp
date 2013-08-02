#include "server.hpp"

namespace wot
{

server::server()
{
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    m_log->write(wot::log::type::info) << "Starting Server." << std::endl;

    m_portno = 80;
    m_buffer_size = 1024;
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    bzero((char*)&m_serv_addr, sizeof(m_serv_addr));
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    m_serv_addr.sin_port = htons(m_portno);

    bind(m_sockfd, (sockaddr*)&m_serv_addr, sizeof(m_serv_addr));
    m_log->write(wot::log::type::info) << "Socket Bound." << std::endl;

    listen(m_sockfd, 5);
    m_log->write(wot::log::type::info) << "Listening to socket." << std::endl;

    m_clilen = sizeof(m_cli_addr);

    m_log->write(wot::log::type::info) << "Server Started." << std::endl;
}

void server::handle()
{
    try
    {
        m_log->write(wot::log::type::info) << "Waiting for a request." << std::endl;

        m_buffer = new char[m_buffer_size];
        bzero(m_buffer, m_buffer_size);

        m_newsockfd = accept(m_sockfd, (sockaddr*)&m_cli_addr, &m_clilen);
        bzero(m_buffer, m_buffer_size);

        m_log->write(wot::log::type::info) << "-------------" << std::endl;

        char inet_str[INET_ADDRSTRLEN];
        inet_ntop(m_cli_addr.sin_family, &(m_cli_addr.sin_addr), inet_str, INET_ADDRSTRLEN);
        m_log->write(wot::log::type::info) << "Request accepted from: " << inet_str << std::endl;

        read(m_newsockfd, m_buffer, m_buffer_size - 1);

        std::string buffer_str(m_buffer);

        m_log->write(wot::log::type::info) << "Packet: " << std::endl << buffer_str << std::endl;

        wot::request client_request(buffer_str);

        if(client_request.errors() == false)
        {
            std::string response;

            if( (client_request.get_host() == "www.cznp.com" || client_request.get_host() == "cznp.com") && client_request.get_method() == "GET" && client_request.get_path() == "/")
            {
                response = wot::response(wot::utils::file_to_string("/home/cznp/index.html"), "text/html");
            }
            else
            {
                response = wot::response(wot::utils::file_to_string("/home/cznp/404.html"), "text/html");
            }

            write(m_newsockfd, response.c_str(), response.size());
        }

        close(m_newsockfd);
        delete [] m_buffer;
    }
    catch(...)
    {
        std::cout << m_buffer << std::endl;
        close(m_newsockfd);
        delete [] m_buffer;
        return;
    }
}

server::~server()
{
    close(m_newsockfd);
    close(m_sockfd);

    delete [] m_buffer;
}

}

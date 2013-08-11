#include "server.hpp"

namespace wot
{

server::server()
{
    // Init the logger
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    m_log->write(wot::log::type::info) << "Starting Server." << std::endl;

    // Init the domain config loading
    m_domains = std::unique_ptr<wot::domains>(new wot::domains());
    if(m_domains->errors())
    {
        m_log->write(wot::log::type::error) << "Error loading allowed domains config." << std::endl;
        exit(1);
    }

    m_log->write(wot::log::type::info) << "Allowed domains config loaded." << std::endl;

    // Create the socket and set the socket options
    m_portno = 80;
    m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Set the current server address information
    bzero((char*)&m_serv_addr, sizeof(m_serv_addr));
    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;
    m_serv_addr.sin_port = htons(m_portno);

    // Bind the socket with the address information given above
    bind(m_sockfd, (sockaddr*)&m_serv_addr, sizeof(m_serv_addr));
    m_log->write(wot::log::type::info) << "Socket Bound." << std::endl;

    // Listen on the socket, with possible 20 connections that can wait in the backlog
    listen(m_sockfd, 20);
    m_log->write(wot::log::type::info) << "Listening to socket." << std::endl;

    m_clilen = sizeof(m_cli_addr);
    m_log->write(wot::log::type::info) << "Server Started." << std::endl;
}

void server::accept_request()
{
    // Accept socket request
    m_log->write(wot::log::type::info) << "Waiting for a request." << std::endl;

    // Accept the the request
    m_newsockfd = accept(m_sockfd, (sockaddr*)&m_cli_addr, &m_clilen);

    // Log who is requesting the data
    char inet_str[INET_ADDRSTRLEN];
    inet_ntop(m_cli_addr.sin_family, &(m_cli_addr.sin_addr), inet_str, INET_ADDRSTRLEN);
    m_log->write(wot::log::type::info) << "Request accepted from: " << inet_str << std::endl;
}

void server::read_request(size_t& read_result, std::string& buffer_str)
{
    // Create and zero out the buffer
    char buffer[1024];
    bzero(buffer, 1024);

    // Read request into a string
    read_result = read(m_newsockfd, buffer, 1024 - 1);
    buffer_str = buffer;
}

void server::handle()
{
    try
    {
        accept_request();

        std::string request_str;
        size_t read_result;
        read_request(read_result, request_str);

        // If the read result isn't ok
        if(read_result != 0)
        {
            m_log->write(wot::log::type::error) << "Error reading from socket: " << read_result << std::endl;
            m_log->write(wot::log::type::error) << request_str << std::endl;

            close(m_newsockfd);
            return;
        }

        // The request string is empty
        if(request_str.empty())
        {
            m_log->write(wot::log::type::error) << "Request string empty!" <<  std::endl;

            close(m_newsockfd);
            return;
        }

        m_log->write(wot::log::type::info) << "Request string: " << std::endl << request_str << std::endl;

        // Parse request string
        wot::request client_request(request_str);
        if(client_request.errors())
        {
            m_log->write(wot::log::type::error) << "Error parsing request string." <<  std::endl;

            close(m_newsockfd);
            return;
        }

        // Load domain configuration
        std::string response = "";
        std::shared_ptr<domain> domain = m_domains->get_domain(client_request.get_host());

        if(!domain)
        {
            m_log->write(wot::log::type::warning) << "Domain: " << client_request.get_host() << " not in allowed list!" << std::endl;

            close(m_newsockfd);
            return;
        }

        if(domain->is_hostname(client_request.get_host()) && client_request.get_method() == "GET")
        {
            wot::path path = domain->get_path(client_request.get_path());
            std::string location = domain->get_location();

            m_log->write(wot::log::type::info) << "Request: " << location << " - " << path.request << " - " << path.file << std::endl;

            if(client_request.get_path().find("/static/") == 0)
            {
                std::string file_path = location + client_request.get_path();
                m_log->write(wot::log::type::info) << "Request for static file: " << file_path << std::endl;

                std::string file_data = wot::utils::file_to_string(file_path.c_str());

                if(file_data.size() > 0)
                {
                    response = wot::response(wot::utils::file_to_string(file_path.c_str()), wot::utils::mime_type(client_request.get_path()));
                }
            }
            else if(path.request.size() > 0 && path.type == wot::path_type::plain)
            {
                std::string file_path = location + "/" +  path.file;
                m_log->write(wot::log::type::info) << "Request for plain file: " << file_path << std::endl;

                response = wot::response(wot::utils::file_to_string(file_path.c_str()), "text/html");
            }
            else if(path.request.size() > 0 && path.type == wot::path_type::python)
            {
                m_log->write(wot::log::type::info) << "Request for python file: " << path.file << std::endl;
                std::string file_path = "python " + location + "/" +  path.file;

                std::string data = wot::utils::program_to_string(file_path);

                response = wot::response(data, "text/html");
            }
        }

        if(response.size() == 0)
        {
            std::string file_path = domain->get_location() + "/" + domain->get_404();

            response = wot::response(wot::utils::file_to_string(file_path.c_str()), "text/html");
        }

        write(m_newsockfd, response.c_str(), response.size());
    }
    catch(...)
    {
        close(m_newsockfd);
        return;
    }
}

server::~server()
{
    close(m_newsockfd);
    close(m_sockfd);
}

}

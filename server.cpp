#include "server.hpp"

namespace wot
{

server::server()
{
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    m_log->write(wot::log::type::info) << "Starting Server." << std::endl;

    m_domains = std::unique_ptr<wot::domains>(new wot::domains());
    if(m_domains->errors())
    {
        m_log->write(wot::log::type::info) << "Error loading allowed domains config." << std::endl;
        exit(1);
    }

    m_log->write(wot::log::type::info) << "Allowed domains config loaded." << std::endl;

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

        size_t read_result = read(m_newsockfd, m_buffer, m_buffer_size - 1);

        if(read_result >= 1)
        { 
            std::string buffer_str(m_buffer);
    
            m_log->write(wot::log::type::info) << "Packet: " << std::endl << buffer_str << std::endl;
    
            wot::request client_request(buffer_str);

            if(client_request.errors() == false)
            {
                std::string response = "";
                std::shared_ptr<domain> domain = m_domains->get_domain(client_request.get_host());

                if(domain)
                {
                    if(domain->is_hostname(client_request.get_host()) && client_request.get_method() == "GET")
                    {
                        wot::path path = domain->get_path(client_request.get_path());
                        std::string location = domain->get_location();
                        
                        m_log->write(wot::log::type::info) << "Request: " << location << " - " << path.request << " - " << path.file << std::endl;
    
                        if(path.request.size() > 0 && path.type == wot::path_type::plain)
                        {
                            std::string file_path = location + "/" +  path.file;
                            m_log->write(wot::log::type::info) << "Request for plain file: " << file_path << std::endl;
    
                            response = wot::response(wot::utils::file_to_string(file_path.c_str()), "text/html");
                        }
                        
                        if(path.request.size() > 0 && path.type == wot::path_type::python)
                        {
                            m_log->write(wot::log::type::info) << "Request for python file: " << path.file << std::endl;
                            std::string file_path = "python " + location + "/" +  path.file;
    
                            FILE* in;
                            if((in = popen(file_path.c_str(), "r")))
                            {
                                char buf[1024];
                                std::stringstream ss;
    
                                while(fgets(buf, sizeof(buf), in) != NULL)
                                {
                                    ss << buf;
                                }
    
                                response = wot::response(ss.str(), "text/html");
    
                                pclose(in);
                            }
                        }
                    }
    
                    if(response.size() == 0)
                    {
                        std::string file_path = domain->get_location() + "/" + domain->get_404();
    
                        response = wot::response(wot::utils::file_to_string(file_path.c_str()), "text/html");
                    }
    
                    write(m_newsockfd, response.c_str(), response.size());
                }
                else
                {
                    m_log->write(wot::log::type::warning) << "Domain: " << client_request.get_host() << " not in allowed list!" << std::endl;
                }
            }
            else
            {
                m_log->write(wot::log::type::warning) << "Error parsing client request." << std::endl;
            }
        }
        else
        {
            m_log->write(wot::log::type::warning) << "Read returned: " << read_result << std::endl;
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

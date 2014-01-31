#include "server.hpp"

namespace wot
{

server::server(int sockfd)
{
    // Init the logger
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));

    m_sockfd = sockfd;
    
    // Init the domain config loading
    m_domains = std::unique_ptr<wot::domains>(new wot::domains());
    if(m_domains->errors())
    {
        m_log->error() << "Error loading allowed domains config." << std::endl;
        exit(1);
    }
}

void server::read_request(size_t& read_result, std::string& buffer_str)
{
    m_log->info() << "Starting to read the request" << std::endl;

    const unsigned int read_size = 8192;

    // Create and zero out the buffer
    char buffer[read_size];
    bzero(buffer, read_size);

    char p;
    size_t peek_size = recv(m_sockfd, &p, 1, MSG_PEEK);
    if(peek_size < 1)
    {
        m_log->info() << "Peek returned: " << peek_size << std::endl;
        
        read_result = peek_size;
        return;
    }

    m_log->info() << "Peek confirmed, data to read" << std::endl;

    read_result = read(m_sockfd, buffer, read_size - 1);
    buffer_str = buffer;
}

std::string server::static_file_response(std::shared_ptr<domain> domain, const wot::request& request)
{
    std::string response = "";

    // Get the file path
    std::string file_path = domain->get_location() + request.get_path();
    m_log->info() << "Request for static file: " << file_path << std::endl;

    // Open the file and put it into a string
    std::string file_data = wot::utils::file_to_string(file_path.c_str());

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, wot::utils::mime_type(request.get_path()));
    }

    return response;
}

std::string server::plain_file_response(std::shared_ptr<domain> domain, const wot::path& path)
{
    std::string response = "";

    std::string file_path = domain->get_location()  + "/" +  path.file;
    m_log->info() << "Request for plain file: " << file_path << std::endl;

    // Open the file and put it into a string
    std::string file_data = wot::utils::file_to_string(file_path.c_str());

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, wot::utils::mime_type(path.file));
    }

    return response;
}

std::string server::python_response(std::shared_ptr<domain> domain, const wot::path& path, std::string post_data)
{
    std::string response = "";

    if(post_data != "")
    {
        post_data = wot::utils::base64_encode(post_data);
    }

    std::string file_path = "python " + domain->get_location() + "/" +  path.file + " " + post_data;
    m_log->info() << "Request for python file: " << file_path << std::endl;

    // Open the file and put it into a string
    std::string file_data = wot::utils::program_to_string(file_path);

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, "text/html");
    }

    return response;
}

std::string server::binary_response(std::shared_ptr<domain> domain, const wot::path& path, const wot::request& request, std::string post_data)
{
    std::string response = "";
    
    if(post_data != "")
    {
        post_data = wot::utils::base64_encode(post_data);
    }

    std::string sanitized_get_path = wot::utils::sanitize_string(request.get_path());

    std::string file_path = domain->get_location() + "/" +  path.file + " " + sanitized_get_path + " " + post_data;
    m_log->info() << "Request for binary file: " << file_path << " with argument " << sanitized_get_path << " (" << request.get_path() << ")" << std::endl;

    // Open the file and put it into a string
    std::string file_data = wot::utils::program_to_string(file_path);

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, "text/html");
    }

    return response;
}

std::string server::file_not_found_response(std::shared_ptr<domain> domain)
{
    std::string file_path = domain->get_location() + "/" + domain->get_404();

    return wot::response(wot::utils::file_to_string(file_path.c_str()), "text/html");
}

void server::handle()
{
    try
    {
        std::string request_str = "";
        size_t read_result = 0;
        read_request(read_result, request_str);

        // If the read result isn't ok
        if(read_result == -1)
        {
            m_log->error() << "Error reading from socket: " << read_result << std::endl;
            m_log->error() << request_str << std::endl;

            close(m_sockfd);
            return;
        }

        // The request string is empty
        if(request_str.empty())
        {
            m_log->error() << "Request string empty!" <<  std::endl;

            close(m_sockfd);
            return;
        }

        m_log->info() << "Request string: " << std::endl << request_str << std::endl;

        // Parse request string
        wot::request client_request(request_str);
        if(client_request.errors())
        {
            m_log->error() << "Error parsing request string." <<  std::endl;

            close(m_sockfd);
            return;
        }

        // Load domain configuration
        std::string response = "";
        std::shared_ptr<domain> domain = m_domains->get_domain(client_request.get_host());

        if(!domain)
        {
            m_log->error() << "Domain: " << client_request.get_host() << " not in allowed list!" << std::endl;

            close(m_sockfd);
            return;
        }

        // If this request is a hostname we are serving and this is a GET request
        if(domain->is_hostname(client_request.get_host()))
        {
            if(client_request.get_method() == "GET")
            {
                // Get path information from the request
                wot::path path = domain->get_path(wot::method_type::get, client_request.get_path());

                // If the path type is unknown, we might have a wildcard type
                if(path.type == wot::path_type::unknown)
                {
                    path = domain->find_wildcard_path(wot::method_type::get, client_request.get_path());
                }

                m_log->info() << "Get Request: " << domain->get_location() << " - " << path.request << " - " << path.file << std::endl;

                // Request is for a static file, we get that response
                if(client_request.get_path().find("/static/") == 0)
                {
                    response = static_file_response(domain, client_request);
                }
                // Request is for a plain file
                else if(path.request.size() > 0 && path.type == wot::path_type::plain)
                {
                    response = plain_file_response(domain, path);
                }
                // Request is for a python file
                else if(path.request.size() > 0 && path.type == wot::path_type::python)
                {
                    response = python_response(domain, path);
                }
                // Request is for a binary file
                else if(path.request.size() > 0 && path.type == wot::path_type::binary)
                {
                    response = binary_response(domain, path, client_request);
                }
            }
            else if(client_request.get_method() == "POST")
            {
                // Get path information from the request
                wot::path path = domain->get_path(wot::method_type::post, client_request.get_path());

                // If the path type is unknown, we might have a wildcard type
                if(path.type == wot::path_type::unknown)
                {
                    path = domain->find_wildcard_path(wot::method_type::post, client_request.get_path());
                }

                m_log->info() << "Post Request: " << domain->get_location() << " - " << path.request << " - " << path.file << std::endl;
                if(path.request.size() > 0 && path.type == wot::path_type::python)
                {
                    response = python_response(domain, path, client_request.get_post_data());
                }
            }
        }

        // 404, response is empty.
        if(response.size() == 0)
        {
            response = file_not_found_response(domain);
        }

        write(m_sockfd, response.c_str(), response.size());
    }
    catch(...)
    {
        close(m_sockfd);
        return;
    }
}

server::~server()
{
    close(m_sockfd);
}

}

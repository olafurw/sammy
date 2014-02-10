#include "server.hpp"

namespace wot
{

server::server(std::string client_address, int sockfd)
{
    m_sockfd = sockfd;
    m_client_address = client_address;
    
    // Init the domain config loading
    m_domains = std::unique_ptr<wot::domains>(new wot::domains());
    if(m_domains->errors())
    {
        exit(1);
    }

    m_log = std::unique_ptr<wot::log>(new wot::log());
}

void server::read_request(size_t& read_result, std::string& buffer_str)
{
    const unsigned int read_size = 8192;

    // Create and zero out the buffer
    char buffer[read_size];
    bzero(buffer, read_size);

    char p;
    size_t peek_size = recv(m_sockfd, &p, 1, MSG_PEEK);
    if(peek_size < 1)
    {
        read_result = peek_size;
        return;
    }

    read_result = read(m_sockfd, buffer, read_size - 1);
    buffer_str = buffer;
}

std::string server::static_file_response(std::shared_ptr<domain> domain, const wot::request& request)
{
    std::string response = "";

    // Get the file path
    std::string file_path = domain->get_location() + request.get_path();

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
            close(m_sockfd);
            return;
        }

        // The request string is empty
        if(request_str.empty())
        {
            close(m_sockfd);
            return;
        }

        // Parse request string
        wot::request client_request(request_str);
        if(client_request.errors())
        {
            close(m_sockfd);
            return;
        }

        // Load domain configuration
        std::string response = "";
        std::shared_ptr<domain> domain = m_domains->get_domain(client_request.get_host());

        if(!domain)
        {
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

                if(path.request.size() > 0 && path.type == wot::path_type::python)
                {
                    response = python_response(domain, path, client_request.get_post_data());
                }
            }
        }

        // 404, response is empty.
        if(response.size() == 0)
        {
            m_log->info(m_client_address + " : " + client_request.get_host() + " : " + client_request.get_method() + " : " + client_request.get_path() + " : SC 404");
            response = file_not_found_response(domain);
        }
        else
        {
            m_log->info(m_client_address + " : " + client_request.get_host() + " : " + client_request.get_method() + " : " + client_request.get_path() + " : SC 200 : " + std::to_string(response.size()) + " bytes");
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

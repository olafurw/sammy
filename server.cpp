#include "server.hpp"

namespace wot
{

server::server(std::shared_ptr<wot::domain> domain,
           std::shared_ptr<wot::request> request, 
           std::string client_address, 
           int sockfd)
{
    m_domain = domain;
    m_request = request;
    m_sockfd = sockfd;
    m_client_address = client_address;
    
    m_log = std::unique_ptr<wot::log>(new wot::log());
}

std::string server::static_file_response()
{
    std::string response = "";

    // Get the file path
    std::string file_path = m_domain->get_location() + m_request->get_path();

    // Open the file and put it into a string
    std::string file_data = wot::utils::file_to_string(file_path.c_str());

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, wot::utils::mime_type(m_request->get_path()));
    }

    return response;
}

std::string server::plain_file_response(const wot::path& path)
{
    std::string response = "";

    std::string file_path = m_domain->get_location()  + "/" +  path.file;

    // Open the file and put it into a string
    std::string file_data = wot::utils::file_to_string(file_path.c_str());

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, wot::utils::mime_type(path.file));
    }

    return response;
}

std::string server::python_response(const wot::path& path, std::string post_data)
{
    std::string response = "";

    if(post_data != "")
    {
        post_data = wot::utils::base64_encode(post_data);
    }

    std::string file_path = "python " + m_domain->get_location() + "/" +  path.file + " " + post_data;

    // Open the file and put it into a string
    std::string file_data = wot::utils::program_to_string(file_path);

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, "text/html");
    }

    return response;
}

std::string server::binary_response(const wot::path& path, std::string post_data)
{
    std::string response = "";
    
    if(post_data != "")
    {
        post_data = wot::utils::base64_encode(post_data);
    }

    std::string sanitized_get_path = wot::utils::sanitize_string(m_request->get_path());

    std::string file_path = m_domain->get_location() + "/" +  path.file + " " + sanitized_get_path + " " + post_data;

    // Open the file and put it into a string
    std::string file_data = wot::utils::program_to_string(file_path);

    if(file_data.size() > 0)
    {
        response = wot::response(file_data, "text/html");
    }

    return response;
}

std::string server::file_not_found_response()
{
    std::string file_path = m_domain->get_location() + "/" + m_domain->get_404();

    return wot::response(wot::utils::file_to_string(file_path.c_str()), "text/html");
}

void server::handle()
{
    try
    {
        std::string response = "";

        // If this request is a hostname we are serving and this is a GET request
        if(m_domain->is_hostname(m_request->get_host()))
        {
            if(m_request->get_method() == "GET")
            {
                // Get path information from the request
                wot::path path = m_domain->get_path(wot::method_type::get, m_request->get_path());

                // If the path type is unknown, we might have a wildcard type
                if(path.type == wot::path_type::unknown)
                {
                    path = m_domain->find_wildcard_path(wot::method_type::get, m_request->get_path());
                }

                // Request is for a static file, we get that response
                if(m_request->get_path().find("/static/") == 0)
                {
                    response = static_file_response();
                }
                // Request is for a plain file
                else if(path.request.size() > 0 && path.type == wot::path_type::plain)
                {
                    response = plain_file_response(path);
                }
                // Request is for a python file
                else if(path.request.size() > 0 && path.type == wot::path_type::python)
                {
                    response = python_response(path);
                }
                // Request is for a binary file
                else if(path.request.size() > 0 && path.type == wot::path_type::binary)
                {
                    response = binary_response(path);
                }
            }
            else if(m_request->get_method() == "POST")
            {
                // Get path information from the request
                wot::path path = m_domain->get_path(wot::method_type::post, m_request->get_path());

                // If the path type is unknown, we might have a wildcard type
                if(path.type == wot::path_type::unknown)
                {
                    path = m_domain->find_wildcard_path(wot::method_type::post, m_request->get_path());
                }

                if(path.request.size() > 0 && path.type == wot::path_type::python)
                {
                    response = python_response(path, m_request->get_post_data());
                }
            }
        }

        // 404, response is empty.
        if(response.size() == 0)
        {
            m_log->info(m_client_address + " : " + m_request->get_host() + " : " + m_request->get_method() + " : " + m_request->get_path() + " : 404" + " : " + m_request->get_referer() + " : " + m_request->get_cookies());
            response = file_not_found_response();
        }
        else
        {
            m_log->info(m_client_address + " : " + m_request->get_host() + " : " + m_request->get_method() + " : " + m_request->get_path() + " : 200 : " + std::to_string(response.size()) + " : " + m_request->get_referer() + " : " + m_request->get_cookies());
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

#include "server.hpp"

namespace sammy
{

server::server(std::shared_ptr<sammy::thread::task> task)
{
    m_domain = task->domain;
    m_request = task->client_request;
    m_sockfd = task->newsockfd;
    m_client_address = task->client_address;
    
    m_log = std::unique_ptr<sammy::log>(new sammy::log());
}

std::string server::static_file_response()
{
    std::string response = "";

    // Get the file path
    std::string file_path = m_domain->get_location() + m_request->get_path();

    sammy::response_data data;

    data.modification_time = sammy::utils::file_modified(file_path.c_str());
    if(data.modification_time <= m_request->get_if_modified_since())
    {
        return sammy::response_304();
    }

    data.message = sammy::utils::file_to_string(file_path.c_str());
    data.type = sammy::utils::mime_type(m_request->get_path());
    data.response_code = 404;

    if(data.message.size() > 0)
    {
        data.response_code = 200;
        response = sammy::response(data);
    }

    return response;
}

std::string server::plain_file_response(const sammy::path& path)
{
    std::string response = "";

    std::string file_path = m_domain->get_location()  + "/" +  path.file;

    sammy::response_data data;

    // Open the file and put it into a string
    data.message = sammy::utils::file_to_string(file_path.c_str());
    data.modification_time = sammy::utils::file_modified(file_path.c_str());
    data.type = sammy::utils::mime_type(path.file);
    data.response_code = 404;

    if(data.message.size() > 0)
    {
        data.response_code = 200;
        response = sammy::response(data);
    }

    return response;
}

std::string server::python_response(const sammy::path& path, std::string post_data)
{
    std::string response = "";

    if(post_data != "")
    {
        post_data = sammy::utils::base64_encode(post_data);
    }

    std::string file_path = "python " + m_domain->get_location() + "/" +  path.file + " " + post_data;

    sammy::response_data data;

    // Open the file and put it into a string
    data.message = sammy::utils::program_to_string(file_path);
    data.modification_time = sammy::utils::current_time();
    data.type = "text/html";
    data.response_code = 404;

    if(data.message.size() > 0)
    {
        data.response_code = 200;
        response = sammy::response(data);
    }

    return response;
}

std::string server::binary_response(const sammy::path& path, std::string post_data)
{
    std::string response = "";
    
    if(post_data != "")
    {
        post_data = sammy::utils::base64_encode(post_data);
    }

    std::string sanitized_get_path = sammy::utils::sanitize_string(m_request->get_path());

    std::string file_path = m_domain->get_location() + "/" +  path.file + " " + sanitized_get_path + " " + post_data;

    sammy::response_data data;

    // Open the file and put it into a string
    data.message = sammy::utils::program_to_string(file_path);
    data.modification_time = sammy::utils::current_time();
    data.type = "text/html";
    data.response_code = 404;

    if(data.message.size() > 0)
    {
        data.response_code = 200;
        response = sammy::response(data);
    }

    return response;
}

std::string server::file_not_found_response()
{
    std::string file_path = m_domain->get_location() + "/" + m_domain->get_404();

    sammy::response_data data;
    data.message = sammy::utils::file_to_string(file_path.c_str());
    data.modification_time = sammy::utils::file_modified(file_path.c_str());
    data.type = "text/html";
    data.response_code = 404;

    return sammy::response(data);
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
                sammy::path path = m_domain->get_path(sammy::method_type::get, m_request->get_path());

                // If the path type is unknown, we might have a wildcard type
                if(path.type == sammy::path_type::unknown)
                {
                    path = m_domain->find_wildcard_path(sammy::method_type::get, m_request->get_path());
                }

                // Request is for a static file, we get that response
                if(m_request->get_path().find("/static/") == 0)
                {
                    response = static_file_response();
                }
                // Request is for a plain file
                else if(path.request.size() > 0 && path.type == sammy::path_type::plain)
                {
                    response = plain_file_response(path);
                }
                // Request is for a python file
                else if(path.request.size() > 0 && path.type == sammy::path_type::python)
                {
                    response = python_response(path);
                }
                // Request is for a binary file
                else if(path.request.size() > 0 && path.type == sammy::path_type::binary)
                {
                    response = binary_response(path);
                }
            }
            else if(m_request->get_method() == "POST")
            {
                // Get path information from the request
                sammy::path path = m_domain->get_path(sammy::method_type::post, m_request->get_path());

                // If the path type is unknown, we might have a wildcard type
                if(path.type == sammy::path_type::unknown)
                {
                    path = m_domain->find_wildcard_path(sammy::method_type::post, m_request->get_path());
                }

                if(path.request.size() > 0 && path.type == sammy::path_type::python)
                {
                    response = python_response(path, m_request->get_post_data());
                }
            }
        }

        // 404, response is empty.
        if(response.size() == 0)
        {
            m_log->info(m_client_address + " : " + m_request->get_host() + " : " + m_request->get_method() + " : " + m_request->get_path() + " : 404" + " : " + m_request->get_referer());
            response = file_not_found_response();
        }
        else
        {
            m_log->info(m_client_address + " : " + m_request->get_host() + " : " + m_request->get_method() + " : " + m_request->get_path() + " : 200 : " + std::to_string(response.size()) + " : " + m_request->get_referer());
        }

        write(m_sockfd, response.c_str(), response.size());
        close(m_sockfd);
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

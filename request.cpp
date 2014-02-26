#include "request.hpp"

namespace sammy
{
request::request(const std::string& r)
{
    m_error = 0;
    m_method = "";
    m_path = "";
    m_host = "";

    m_request_lines = sammy::utils::split_string(r, '\n');

    if(m_request_lines.size() == 0)
    {
        m_error = 1;
        return;
    }
    
    parse_header();

    for(unsigned int i = 1; i < m_request_lines.size(); ++i)
    {
        // Find the line type of the request, to route it to the correct parse function
        std::string& current_line = m_request_lines.at(i);
        auto colon_pos = current_line.find(": ");
        if(colon_pos == std::string::npos)
        {
            continue;
        }

        std::string line_type = current_line.substr(0, colon_pos);
        std::string line_data = current_line.substr(colon_pos + 2);

        if(line_type == "Host")
        {
            parse_host(line_data);

            continue;
        }

        if(line_type == "Cookie")
        {
            parse_cookies(line_data);

            continue;
        }

        if(line_type == "Referer")
        {
            parse_referer(line_data);

            continue;
        }
    }    

    parse_post_data();
    
    if(m_error == 1 || m_method.size() == 0 || m_path.size() == 0 || m_method.size() == 0)
    {
        m_error = 1;
        return;
    }
}

void request::parse_header()
{
    std::vector<std::string> first_line = sammy::utils::split_string(m_request_lines.at(0), ' ');

    if(first_line.size() < 3)
    {
        return;
    }

    std::string& method = first_line.at(0);

    if(method == "GET" || method == "POST")
    {   
        m_method = method;
    }
    else
    {
        m_error = 1;
        m_method = "UNKNOWN";
    }
    
    m_path = sammy::utils::trim(first_line.at(1));

    return;
}

void request::parse_host(const std::string& host_data)
{
    m_host = sammy::utils::trim(host_data);
}

void request::parse_cookies(const std::string& cookie_data)
{
    auto cookies = sammy::utils::split_string(cookie_data, ';');

    for(auto& cookie : cookies)
    {
        auto eq_pos = cookie.find("=");
        std::string key = sammy::utils::trim(cookie.substr(0, eq_pos));
        std::string value = sammy::utils::trim(cookie.substr(eq_pos + 1));

        if(key == "" || value == "")
        {
            continue;
        }

        m_cookies[key] = value;
    }
}

void request::parse_post_data()
{
    if(m_method != "POST")
    {
        return;
    }

    std::stringstream post_stream;

    bool found_empty_line = false;
    for(const auto& line : m_request_lines)
    {
        if(!found_empty_line && sammy::utils::trim(line) == "")
        {
            found_empty_line = true;
        }

        if(found_empty_line)
        {
            post_stream << line;
        }
    }

    m_post_data = sammy::utils::trim(post_stream.str());
}

void request::parse_referer(const std::string& referer_data)
{
    m_referer = sammy::utils::trim(referer_data);
}

std::string request::get_host() const
{   
    return m_host;
}

std::string request::get_path() const
{   
    return m_path;
}

std::string request::get_method() const
{   
    return m_method;
}

std::string request::get_post_data() const
{
    return m_post_data;
}

std::string request::get_referer() const
{
    return m_referer;
}

bool request::errors() const
{
    return m_error != 0;
}

}

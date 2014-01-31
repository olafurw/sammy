#include "request.hpp"

namespace wot
{
request::request(const std::string& r)
{
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    m_log->info() << "Started request." << std::endl;

    m_error = 0;
    m_method = "";
    m_path = "";
    m_host = "";

    std::vector<std::string> request_lines = wot::utils::split_string(r, '\n');

    if(request_lines.size() == 0)
    {
        m_error = 1;
        return;
    }
    
    parse_header(request_lines.at(0));
    parse_host(request_lines);
    parse_cookies(request_lines);
    parse_post_data(request_lines);
    
    if(m_error == 1 || m_method.size() == 0 || m_path.size() == 0 || m_method.size() == 0)
    {
        m_error = 1;
        return;
    }

    m_log->info() << "Request parsed: " << m_method << " " << m_host << " " << m_path << std::endl;
}

void request::parse_header(const std::string& header)
{
    std::vector<std::string> first_line = wot::utils::split_string(header, ' ');

    if(first_line.size() < 3)
    {
        return;
    }

    if(first_line.at(0) == "GET")
    {   
        m_method = "GET";
    }
    else if(first_line.at(0) == "POST")
    {
        m_method = "POST";
    }
    else
    {
        m_error = 1;
        m_method = "UNKNOWN";
    }
    
    m_path = wot::utils::trim(first_line.at(1));

    return;
}

void request::parse_host(const std::vector<std::string>& request_lines)
{
    int hostindex = wot::utils::line_starting_with("Host:", request_lines);
    if(hostindex == -1)
    {
        m_error = 1;
        return;
    }

    auto hostline = wot::utils::split_string(request_lines.at(hostindex), ' ');
    if(hostline.size() >= 2)
    {   
        m_host = wot::utils::trim(hostline.at(1));
        return;
    }

    m_error = 1;
    
    return;
}

void request::parse_cookies(const std::vector<std::string>& request_lines)
{
    int cookieindex = wot::utils::line_starting_with("Cookie:", request_lines);
    if(cookieindex == -1)
    {
        return;
    }
    
    m_log->info() << "Cookie: " << request_lines.at(cookieindex) << std::endl;
}

void request::parse_post_data(const std::vector<std::string>& request_lines)
{
    if(m_method != "POST")
    {
        return;
    }

    std::stringstream post_stream;

    bool found_empty_line = false;
    for(const auto& line : request_lines)
    {
        if(!found_empty_line && wot::utils::trim(line) == "")
        {
            found_empty_line = true;
        }

        if(found_empty_line)
        {
            post_stream << line;
        }
    }

    m_post_data = wot::utils::trim(post_stream.str());
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

bool request::errors() const
{
    return m_error != 0;
}

}

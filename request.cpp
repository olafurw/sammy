#include "request.hpp"

namespace wot
{
request::request(const std::string& r)
{
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    m_log->write(wot::log::type::info) << "Started request." << std::endl;

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
    
    std::vector<std::string> first_line = wot::utils::split_string(request_lines.at(0), ' ');

    if(first_line.size() < 3)
    {
        m_error = 1;
        return;
    }

    if(first_line.at(0) == "GET")
    {   
        m_method = "GET";
    }
    
    m_path = wot::utils::trim(first_line.at(1));
    
    int m_hostindex = find_line_starting_with("Host:", request_lines);
    std::vector<std::string> m_hostline = wot::utils::split_string(request_lines.at(m_hostindex), ' ');
    if(m_hostline.size() >= 2)
    {   
        m_host = wot::utils::trim(m_hostline.at(1));
    }

    if(m_method.size() == 0 || m_path.size() == 0 || m_method.size() == 0)
    {
        m_error = 1;
        return;
    }

    m_log->write(wot::log::type::info) << "Request parsed: " << m_method << " " << m_host << " " << m_path << std::endl;
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

bool request::errors() const
{
    return m_error != 0;
}

int request::find_line_containing(const std::string& value, const std::vector<std::string>& request_lines) const
{   
    for(unsigned int i = 0; i < request_lines.size(); ++i)
    {   
        if(request_lines.at(i).find(value) != std::string::npos)
        {
            return i;
        }   
    }   

    return -1; 
}

int request::find_line_starting_with(const std::string& value, const std::vector<std::string>& request_lines) const
{
    for(unsigned int i = 0; i < request_lines.size(); ++i)
    {
        if(request_lines.at(i).find(value) == 0)
        {
            return i;
        }
    }

    return -1;
}
}

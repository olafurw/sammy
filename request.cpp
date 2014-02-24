#include "request.hpp"

namespace wot
{
request::request(const std::string& r)
{
    m_error = 0;
    m_method = "";
    m_path = "";
    m_host = "";

    m_request_lines = wot::utils::split_string(r, '\n');

    if(m_request_lines.size() == 0)
    {
        m_error = 1;
        return;
    }
    
    parse_header();
    parse_host();
    parse_cookies();
    parse_post_data();
    parse_referer();
    
    if(m_error == 1 || m_method.size() == 0 || m_path.size() == 0 || m_method.size() == 0)
    {
        m_error = 1;
        return;
    }
}

void request::parse_header()
{
    std::vector<std::string> first_line = wot::utils::split_string(m_request_lines.at(0), ' ');

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

void request::parse_host()
{
    int hostindex = wot::utils::line_starting_with("Host:", m_request_lines);
    if(hostindex == -1)
    {
        m_error = 1;
        return;
    }

    auto hostline = wot::utils::split_string(m_request_lines.at(hostindex), ' ');
    if(hostline.size() >= 2)
    {   
        m_host = wot::utils::trim(hostline.at(1));
        return;
    }

    m_error = 1;
    
    return;
}

void request::parse_cookies()
{
    int cookieindex = wot::utils::line_starting_with("Cookie:", m_request_lines);
    if(cookieindex == -1)
    {
        return;
    }

    std::string cookieline = m_request_lines.at(cookieindex).substr(7);

    auto cookies = wot::utils::split_string(cookieline, ';');

    for(auto& cookie : cookies)
    {
        auto cookie_kv = wot::utils::split_string(cookie, '=');
        if(cookie_kv.size() != 2)
        {
            continue;
        }

        m_cookies[cookie_kv.at(0)] = cookie_kv.at(1);
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

void request::parse_referer()
{
    int refererindex = wot::utils::line_starting_with("Referer:", m_request_lines);
    if(refererindex == -1)
    {
        return;
    }

    auto refererline = wot::utils::split_string(m_request_lines.at(refererindex), ' ');
    if(refererline.size() >= 2)
    {
        m_referer = wot::utils::trim(refererline.at(1));
    }
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

std::string request::get_cookies() const
{
    std::stringstream ss;

    for(auto& kv : m_cookies)
    {
        ss << kv.first << "=" << kv.second;
    }

    return ss.str();
}

bool request::errors() const
{
    return m_error != 0;
}

}

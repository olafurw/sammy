#include "request.hpp"

namespace wot
{
request::request(const std::string& r)
{
    log_ = std::unique_ptr<wot::log>(new wot::log(std::cout));
    log_->write(wot::log::type::info) << "Started request." << std::endl;    

    error_ = 0;
    method_ = "";
    path_ = "";
    host_ = "";

    std::vector<std::string> request_lines = wot::utils::split_string(r, '\n');

    if(request_lines.size() == 0)
    {
        error_ = 1;
        return;
    }
    
    std::vector<std::string> first_line = wot::utils::split_string(request_lines.at(0), ' ');

    if(first_line.size() < 3)
    {
        error_ = 1;
        return;
    }

    if(first_line.at(0) == "GET")
    {   
        method_ = "GET";
    }
    
    path_ = wot::utils::trim(first_line.at(1));
    
    int host_index = find_line_containing("Host:", request_lines);
    std::vector<std::string> host_line = wot::utils::split_string(request_lines.at(host_index), ' ');
    if(host_line.size() >= 2)
    {   
        host_ = wot::utils::trim(host_line.at(1));
    }

    if(method_.size() == 0 || path_.size() == 0 || method_.size() == 0)
    {
        error_ = 1;
        return;
    }

    log_->write(wot::log::type::info) << "Request parsed: " << method_ << " " << host_ << " " << path_ << std::endl;    
    //wot::log::info("Request parsed: " + method_ + " " + host_ + "" + path_);
}

std::string request::get_host() const
{   
    return host_;
}

std::string request::get_path() const
{   
    return path_;
}

std::string request::get_method() const
{   
    return method_;
}

bool request::errors() const
{
    return error_ != 0;
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
}

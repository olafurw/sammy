#include "response.hpp"

namespace sammy
{
std::string response(const response_data& data)
{
    std::string modification_time = sammy::utils::format_unixtime(data.modification_time, "%c");

    std::stringstream ss;
    ss << "HTTP/1.0 " << data.response_code << " OK\n";
    ss << "Server: sammy v0.1\n";
    ss << "MIME-version: 1.0\n";
    ss << "Content-type: " << data.type << "\n";
    ss << "Access-Control-Allow-Headers:accept, origin, x-requested-with, content-type\n";
    ss << "Access-Control-Allow-Methods:GET, POST, PUT, DELETE, OPTIONS\n";
    ss << "Access-Control-Allow-Origin:*\n";
    ss << "Access-Control-Max-Age:15\n";
    ss << "Set-Cookie: SAMMY_SESSION=abc123; Expires=Wed, 16 Nov 2015 12:34:56 GMT" << "\n";
    ss << "Last-Modified: " << modification_time << "\n";
    ss << "Content-Length: " << data.message.size() << "\n\n";
    ss << data.message << "\n\n";

    return ss.str();
}
    
}

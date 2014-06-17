#include "response.hpp"

namespace sammy
{

std::string server_name()
{
    return R"(Server: sammy v0.1
)";
}

std::string mime_version()
{
    return R"(MIME-version: 1.0
)";
}

std::string http_version()
{
    return R"(HTTP/1.0)";
}

std::string access_control()
{
    return R"(Access-Control-Allow-Headers:accept, origin, x-requested-with, content-type
              Access-Control-Allow-Methods:GET, POST, PUT, DELETE, OPTIONS
              Access-Control-Allow-Origin:*
              Access-Control-Max-Age:15
)";
}

std::string response_304()
{
    std::stringstream ss;
    ss << http_version() << " 304 Not Modified\n";
    ss << server_name();
    ss << mime_version();
    ss << access_control();
    ss << "\n";

    return ss.str();
}

std::string response(const response_data& data)
{
    std::string modification_time = sammy::utils::format_unixtime(data.modification_time, "%c");

    std::stringstream ss;
    ss << http_version() << " " << data.response_code << " OK\n";
    ss << server_name();
    ss << mime_version();
    ss << "Content-type: " << data.type << "\n";
    ss << access_control();
    ss << "Set-Cookie: SAMMY_SESSION=abc123; Expires=Wed, 16 Nov 2015 12:34:56 GMT" << "\n";
    ss << "Last-Modified: " << modification_time << "\n";
    ss << "Content-Length: " << data.message.size() << "\n\n";
    ss << data.message << "\n\n";

    return ss.str();
}
    
}

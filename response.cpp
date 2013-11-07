#include "response.hpp"

namespace wot
{
std::string response(const std::string& message, const std::string& type)
{
    std::unique_ptr<std::string> time = wot::utils::current_time("%c");

    std::stringstream ss;
    ss << "HTTP/1.0 200 OK\n";
    ss << "Server: tiaswot\n";
    ss << "MIME-version: 1.0\n";
    ss << "Content-type: " << type << "\n";
    ss << "Set-Cookie: WOTSESSION=abc123; Expires=Wed, 16 Nov 2013 12:34:56 GMT" << "\n";
    ss << "Last-Modified: " << *time << "\n";
    ss << "Content-Length: " << message.size() << "\n\n";
    ss << message << "\n\n";

    return ss.str();
}
}

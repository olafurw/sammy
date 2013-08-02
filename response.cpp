#include "response.hpp"

namespace wot
{
std::string response(const std::string& message, const std::string& type)
{
    std::stringstream ss;
    ss << "HTTP/1.0 200 OK\n";
    ss << "Server: tiaswot\n";
    ss << "MIME-version: 1.0\n";
    ss << "Content-type: " << type << "\n";
    ss << "Last-Modified: Thu Jul 7 00:25:33 1994\n";
    ss << "Content-Length: " << message.size() << "\n\n";
    ss << message << "\n\n";

    return ss.str();
}
}

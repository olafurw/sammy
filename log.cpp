#include "log.hpp"

namespace wot
{
std::unique_ptr<std::string> current_time()
{
    time_t rawtime;
    tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);

    std::unique_ptr<std::string> result{ new std::string(buffer) };

    return result;
}

log::log(std::ostream& out)
{
    m_out = &out;
}

std::ostream& log::write(wot::log::type log_type)
{
    std::string type_string = "";

    switch(log_type)
    {
        case type::info:
            type_string = "I";
        break;
        case type::warning:
            type_string = "W";
        break;
        case type::error:
            type_string = "E";
        break;
        case type::debug:
            type_string = "D";
        break;
        default:
            type_string = "?";
        break;
    }

    std::unique_ptr<std::string> ctime = current_time();
    *m_out << "[" << type_string << "] " << *ctime << " " << std::flush;
    return *m_out;
}
}

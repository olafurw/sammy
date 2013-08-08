#include "log.hpp"

namespace wot
{
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

    std::unique_ptr<std::string> ctime = wot::utils::current_time("%Y-%m-%d %H:%M:%S");
    *m_out << "[" << type_string << "] " << *ctime << " " << std::flush;
    return *m_out;
}
}

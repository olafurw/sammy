#include "log.hpp"

namespace wot
{
log::log()
{
}

void log::write(wot::log::type log_type, const std::string& data)
{
    std::lock_guard<std::mutex> s_lock(m_mutex);
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

    std::cout << "[" << type_string << "] " << *ctime << " " << data << std::endl;
}

void log::info(const std::string& data)
{
    write(wot::log::type::info, data);
}

void log::error(const std::string& data)
{
    write(wot::log::type::error, data);
}

}

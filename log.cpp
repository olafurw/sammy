#include "log.hpp"

namespace sammy
{
log::log()
{
}

void log::write(sammy::log::type log_type, const std::string& data)
{
    std::lock_guard<std::mutex> s_lock(m_mutex);
    std::string type_string = "";
    std::string log_file = "";

    switch(log_type)
    {
        case type::info:
            type_string = "I";
            log_file = "log.info";
        break;
        case type::warning:
            type_string = "W";
            log_file = "log.warning";
        break;
        case type::error:
            type_string = "E";
            log_file = "log.error";
        break;
        case type::debug:
            type_string = "D";
            log_file = "log.debug";
        break;
        default:
            type_string = "?";
            log_file = "log.unknown";
        break;
    }

    std::unique_ptr<std::string> ctime = sammy::utils::current_time("%Y-%m-%d %H:%M:%S");

    std::string log_data = "[" + type_string + "] " + (*ctime) + " " + data + "\n";
    sammy::utils::write_to_file(log_file.c_str(), log_data);
}

void log::info(const std::string& data)
{
    write(sammy::log::type::info, data);
}

void log::warning(const std::string& data)
{
    write(sammy::log::type::warning, data);
}

void log::debug(const std::string& data)
{
    write(sammy::log::type::debug, data);
}

void log::error(const std::string& data)
{
    write(sammy::log::type::error, data);
}

}

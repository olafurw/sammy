#include "log.hpp"

namespace wot
{
log::log(std::ostream& out)
{
    m_out = &out;
    m_last_flush = 0;
}

std::stringstream& log::write(wot::log::type log_type)
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
    m_ss << "[" << type_string << "] " << *ctime << " ";
    return m_ss;
}

std::stringstream& log::info()
{
    return write(wot::log::type::info);
}

std::stringstream& log::warning()
{
    return write(wot::log::type::warning);
}

std::stringstream& log::error()
{
    return write(wot::log::type::error);
}

std::stringstream& log::debug()
{
    return write(wot::log::type::debug);
}

void log::flush()
{
    std::lock_guard<std::mutex> s_lock(m_mutex);

    time_t current = wot::utils::current_time();

    // Not completely portable
    // If there are 10 seconds since we last flushed, then we do
    if((current - m_last_flush) > 10)
    {
        //*m_out << m_ss.str() << std::flush;
        m_ss.clear();

        m_last_flush = current;
    }
}

}

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
        out_ = &out;
    }

    std::ostream& log::write(wot::log::type log_type)
    {
        std::string type_string = "";

        switch(log_type)
        {
            case type::info:
                type_string = "INFO";
            break;
            case type::warning:
                type_string = "WARNING";
            break;
            case type::error:
                type_string = "ERROR";
            break;
            default:
                type_string = "UNKNOWN";
            break;
        }

        std::unique_ptr<std::string> ctime = current_time();
        *out_ << "[" << type_string << "] " << *ctime << " " << std::flush;
        return *out_;
    }
}

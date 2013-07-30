#include "log.hpp"

namespace wot
{
namespace log
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

    void info(const std::string& message)
    {
        std::unique_ptr<std::string> ctime = current_time();
        std::cout << "[INFO] [" << *ctime << "] " << message << std::endl << std::flush;
    }

    void warning(const std::string& message)
    {
        std::unique_ptr<std::string> ctime = current_time();
        std::cout << "[WARNING] [" << *ctime << "] " << message << std::endl << std::flush;
    }

    void error(const std::string& message)
    {
        std::unique_ptr<std::string> ctime = current_time();
        std::cout << "[ERROR] [" << *ctime << "] " << message << std::endl << std::flush;
    }
}
}

#include "log.hpp"

namespace wot
{
namespace log
{
    char* current_time()
    {
        time_t rawtime;
        tm* timeinfo;
        char* buffer = new char[80];
        
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        
        return buffer;
    }

    void info(const std::string& message)
    {
        char* ctime = current_time();
        std::cout << "[INFO] [" << ctime << "] " << message << std::endl << std::flush;
        delete [] ctime;
    }

    void warning(const std::string& message)
    {
        char* ctime = current_time();
        std::cout << "[WARNING] [" << ctime << "] " << message << std::endl << std::flush;
        delete [] ctime;
    }

    void error(const std::string& message)
    {
        char* ctime = current_time();
        std::cout << "[ERROR] [" << ctime << "] " << message << std::endl << std::flush;
        delete [] ctime;
    }
}
}

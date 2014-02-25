#ifndef _SAMMY_LOG_
#define _SAMMY_LOG_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include <thread>
#include <mutex>

#include "utils/utils.hpp"

namespace sammy
{
class log
{
public:
    enum class type : unsigned int
    {
        info = 1,
        warning = 2,
        error = 3,
        debug = 4
    };

    log();

    void info(const std::string& data);
    void error(const std::string& data);

private:
    void write(sammy::log::type log_type, const std::string& data);

    std::mutex m_mutex;
};
}

#endif

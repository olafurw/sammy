#ifndef _WOT_LOG_
#define _WOT_LOG_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>
#include <thread>
#include <mutex>

#include "utils/utils.hpp"

namespace wot
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

private:
    void write(wot::log::type log_type, const std::string& data);

    std::mutex m_mutex;
};
}

#endif

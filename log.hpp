#ifndef _WOT_LOG_
#define _WOT_LOG_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <sstream>

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

    log(std::ostream& output);

    std::stringstream& info();
    std::stringstream& warning();
    std::stringstream& error();
    std::stringstream& debug();

    void flush();

private:
    std::stringstream& write(wot::log::type log_type);

    std::ostream* m_out;
    std::stringstream m_ss;
    time_t m_last_flush;
};
}

#endif

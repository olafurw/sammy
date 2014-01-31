#ifndef _WOT_LOG_
#define _WOT_LOG_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>

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

    std::ostream& info();
    std::ostream& warning();
    std::ostream& error();
    std::ostream& debug();

private:
    std::ostream& write(wot::log::type log_type);
    std::ostream* m_out;
};
}

#endif

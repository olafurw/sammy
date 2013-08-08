#ifndef _WOT_LOG_
#define _WOT_LOG_

#include <iostream>
#include <string>
#include <fstream>
#include <memory>

#include "utils.hpp"

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

    std::ostream& write(wot::log::type log_type);

private:
    std::ostream* m_out;
};
}

#endif

#ifndef _SAMMY_RESPONSE_
#define _SAMMY_RESPONSE_

#include <string>
#include <sstream>
#include <iostream>

#include "utils/utils.hpp"

namespace sammy
{
struct response_data
{
    int response_code;
    time_t modification_time;
    std::string message;
    std::string type;
};

    std::string response_304();
    std::string response(const response_data& data);
}

#endif

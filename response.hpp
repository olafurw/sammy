#ifndef _SAMMY_RESPONSE_
#define _SAMMY_RESPONSE_

#include <string>
#include <sstream>
#include <iostream>

#include "utils/utils.hpp"

namespace sammy
{
    std::string response(const std::string& message, const std::string& type);
}

#endif

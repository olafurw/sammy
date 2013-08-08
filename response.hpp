#ifndef _WOT_RESPONSE_
#define _WOT_RESPONSE_

#include <string>
#include <sstream>
#include <iostream>

#include "utils.hpp"

namespace wot
{
    std::string response(const std::string& message, const std::string& type);
}

#endif

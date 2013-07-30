#ifndef _WOT_LOG_
#define _WOT_LOG_

#include <iostream>
#include <string>

namespace wot
{
namespace log
{
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
}
}

#endif

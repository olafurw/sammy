#ifndef _WOT_CONFIG_
#define _WOT_CONFIG_

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

namespace wot
{
class domains
{
public:
    domains();
    bool errors();
    bool is_allowed(const std::string& domain);
    std::string path(const std::string& domain);

private:
    std::map<std::string, std::string> m_allowed_domains;
    int m_error;
};
}

#endif

#ifndef _SAMMY_CONFIG_
#define _SAMMY_CONFIG_

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>

#include "domain.hpp"
#include "utils/utils.hpp"

namespace sammy
{
class domains
{
public:
    domains();
    bool errors();

    bool is_domain(const std::string& domain);
    std::shared_ptr<domain> get_domain(const std::string& domain);

private:
    std::map<std::string, std::shared_ptr<domain>> m_domains;

    int m_error;
};
}

#endif

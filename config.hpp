#ifndef _WOT_CONFIG_
#define _WOT_CONFIG_

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>

#include "domain.hpp"
#include "utils/utils.hpp"
#include "log.hpp"

namespace wot
{
class domains
{
public:
    domains();
    bool errors();

    bool is_domain(const std::string& domain);
    std::shared_ptr<domain> get_domain(const std::string& domain);

private:
    std::unique_ptr<wot::log> m_log;
    std::map<std::string, std::shared_ptr<domain>> m_domains;

    int m_error;
};
}

#endif

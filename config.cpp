#include "config.hpp"

namespace wot
{
domains::domains()
{
    m_error = 0;
    std::ifstream domain_file("config/allowed_domains.wl");

    std::string line;
    while(std::getline(domain_file, line))
    {
        if(line.size() == 0)
        {
             break;
        }

        std::stringstream ss(line);
        std::string domain_name;
        std::string domain_path;

        if(!(ss >> domain_name >> domain_path))
        {
            m_error = 1;
            break;
        }
       
        m_allowed_domains[domain_name] = domain_path;
    }
}

bool domains::errors()
{
    return m_error != 0;
}

bool domains::is_allowed(const std::string& domain)
{
    return m_allowed_domains.count(domain) == 1;
}

std::string domains::path(const std::string& domain)
{
    return m_allowed_domains[domain];
}
}

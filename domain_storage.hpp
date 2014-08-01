#ifndef _SAMMY_DOMAIN_STORAGE_
#define _SAMMY_DOMAIN_STORAGE_

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <memory>

#include "domain.hpp"
#include "utils/utils.hpp"

namespace sammy
{
class domain_storage
{
public:
	domain_storage();
    bool errors();

    bool is_domain(const std::string& domain);
    std::shared_ptr<domain> get_domain(const std::string& domain);

private:
    void process_hostnames(const std::string& domain_prefix, std::shared_ptr<domain> domain);
    void process_location(const std::string& domain_prefix, std::shared_ptr<domain> domain);
    void process_paths(const std::string& domain_prefix, std::shared_ptr<domain> domain);
    void process_blacklist(const std::string& domain_prefix, std::shared_ptr<domain> domain);
    void process_404(const std::string& domain_prefix, std::shared_ptr<domain> domain);

    std::map<std::string, std::shared_ptr<domain>> m_domains;

    int m_error;
};
}

#endif

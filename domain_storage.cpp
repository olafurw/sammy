#include "domain_storage.hpp"

namespace sammy
{
domain_storage::domain_storage()
{
    m_error = 0;

    const std::string domain_list_file = "config/domains.wl";
    const auto domains = sammy::utils::file_to_array(domain_list_file.c_str());
    for(const auto& domain_prefix : domains)
    {
        std::shared_ptr<sammy::domain> domain = std::make_shared<sammy::domain>();

        process_hostnames(domain_prefix, domain);
        process_location(domain_prefix, domain);
        process_paths(domain_prefix, domain);
        process_blacklist(domain_prefix, domain);
        process_404(domain_prefix, domain);

        if(m_error != 0)
        {
            break;
        }

        for(const auto& hostname : domain->get_hostnames())
        {
            m_domains[hostname] = domain;
        } 
    }
}

void domain_storage::process_hostnames(const std::string& domain_prefix, std::shared_ptr<domain> domain)
{
    const std::string hostnames_file = "config/" + domain_prefix + ".hostnames";

    const auto hostnames = sammy::utils::file_to_array(hostnames_file.c_str());
    for(auto& hostname : hostnames)
    {
        domain->add_hostname(hostname);
    }
}

void domain_storage::process_location(const std::string& domain_prefix, std::shared_ptr<domain> domain)
{
    const std::string location_file = "config/" + domain_prefix + ".location";

    const std::string location = sammy::utils::file_to_string(location_file.c_str());
    domain->set_location(location);
}

void domain_storage::process_paths(const std::string& domain_prefix, std::shared_ptr<domain> domain)
{
    const std::string path_file = "config/" + domain_prefix + ".paths";

    const auto paths = sammy::utils::file_to_array(path_file.c_str());
    for(auto& path : paths)
    {
        std::vector<std::string> path_parts = sammy::utils::split_string(path, ' ', true);
        if(path_parts.size() != 4)
        {
            m_error = 1;
            return;
        }

        sammy::path domain_path(path_parts[0], path_parts[2], path_parts[1], path_parts[3]);

        domain->add_path(domain_path);
    }
}

void domain_storage::process_blacklist(const std::string& domain_prefix, std::shared_ptr<domain> domain)
{
    const std::string blacklist_file = "config/" + domain_prefix + ".blacklist";

    const auto blacklisted_users = sammy::utils::file_to_array(blacklist_file.c_str());
    for(auto& user : blacklisted_users)
    {
        domain->add_blacklist(user);
    }
}

void domain_storage::process_404(const std::string& domain_prefix, std::shared_ptr<domain> domain)
{
    const std::string file_404 = "config/" + domain_prefix + ".404";
    const std::string data_404 = sammy::utils::trim(sammy::utils::file_to_string(file_404.c_str()));

    domain->set_404(data_404);
}

bool domain_storage::is_domain(const std::string& domain)
{
    return m_domains.find(domain) != m_domains.end();
}

std::shared_ptr<domain> domain_storage::get_domain(const std::string& domain)
{
    return is_domain(domain) ? m_domains[domain] : nullptr;
}

bool domain_storage::errors()
{
    return m_error != 0;
}
}

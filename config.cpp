#include "config.hpp"

namespace wot
{
domains::domains()
{
    m_log = std::unique_ptr<wot::log>(new wot::log(std::cout));
    m_error = 0;

    std::ifstream domain_lists("config/domains.wl");
    std::string domain_prefix;

    while(std::getline(domain_lists, domain_prefix))
    {
        std::shared_ptr<wot::domain> domain = std::make_shared<wot::domain>();

        // Hostnames
        std::ifstream domain_hostnames("config/" + domain_prefix + ".hostnames");
        std::string hostname;
        while(std::getline(domain_hostnames, hostname))
        {
            std::string trimmed_hostname = wot::utils::trim(hostname);

            m_domains[trimmed_hostname] = domain;

            domain->add_hostname(trimmed_hostname);
        }
        domain_hostnames.close();

        m_log->write(wot::log::type::info) << domain_prefix << " Hostname parsing complete." << std::endl;

        // Location
        std::ifstream domain_location("config/" + domain_prefix + ".location");
        std::string location;
        std::getline(domain_location, location);
        domain->set_location(wot::utils::trim(location));
        domain_location.close();

        m_log->write(wot::log::type::info) << domain_prefix << " Location parsing complete." << std::endl;

        // Paths
        std::ifstream domain_paths("config/" + domain_prefix + ".paths");
        std::string path_line;
        while(std::getline(domain_paths, path_line))
        {
            std::stringstream path_ss(path_line);

            wot::path path;
            std::string path_type_text;

            path_ss >> path.request;
            path_ss >> path_type_text;
            path_ss >> path.file;

            if(path_type_text == "plain")
            {
                path.type = wot::path_type::plain;
            }
            else if(path_type_text == "python")
            {
                path.type = wot::path_type::python;
            }
            else if (path_type_text == "binary")
            {
                path.type = wot::path_type::binary;
            }
            else
            {
                path.type = wot::path_type::unknown;
            }

            domain->add_path(path);
        }
        domain_paths.close();
        
        m_log->write(wot::log::type::info) << domain_prefix << " Path parsing complete." << std::endl;

        // 404
        std::ifstream domain_404("config/" + domain_prefix + ".404");
        std::string file_404;
        std::getline(domain_404, file_404);
        domain->set_404(wot::utils::trim(file_404));
        domain_404.close();

        m_log->write(wot::log::type::info) << domain_prefix << " 404 parsing loaded." << std::endl;

        m_log->write(wot::log::type::info) << domain_prefix << " Config loaded." << std::endl;
    }

    domain_lists.close();
}

bool domains::is_domain(const std::string& domain)
{
    return m_domains.find(domain) != m_domains.end();
}

std::shared_ptr<domain> domains::get_domain(const std::string& domain)
{
    return is_domain(domain) ? m_domains[domain] : nullptr;
}

bool domains::errors()
{
    return m_error != 0;
}
}

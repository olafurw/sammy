#include "domain.hpp"

namespace wot
{
domain::domain()
{
}

void domain::add_hostname(const std::string& hostname)
{
    m_hostnames.push_back(hostname);
}

void domain::add_blacklist(const std::string& blacklist)
{
    m_blacklist.push_back(blacklist);
}

void domain::set_location(const std::string& location)
{
    m_location = location;
}

void domain::set_404(const std::string& file_404)
{
    m_404 = file_404;
}

void domain::add_path(const wot::path& path)
{
    m_paths[path.method][path.request] = path;
}

bool domain::is_hostname(const std::string& hostname)
{
    return std::find(m_hostnames.begin(), m_hostnames.end(), hostname) != m_hostnames.end();
}

bool domain::is_blacklisted(const std::string& blacklist)
{
    return std::find(m_blacklist.begin(), m_blacklist.end(), blacklist) != m_blacklist.end();
}

std::string domain::get_location()
{
    return m_location;
}

std::string domain::get_404()
{
    return m_404;
}

wot::path domain::get_path(wot::method_type method, const std::string& path)
{
    return m_paths[method][path];
}

wot::path domain::find_wildcard_path(wot::method_type method, const std::string& path)
{
    wot::path result;

    for(auto& kv: m_paths[method])
    {
        if(wot::utils::ends_with(kv.first, "*"))
        {
            std::string trim_path = kv.first.substr(0, kv.first.size() - 1);

            if(path.find(trim_path) == 0)
            {
                result = kv.second;
                break;
            }
        }
    }

    return result;
}

}

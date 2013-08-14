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
    m_paths[path.request] = path;
}

bool domain::is_hostname(const std::string& hostname)
{
    return std::find(m_hostnames.begin(), m_hostnames.end(), hostname) != m_hostnames.end();
}

std::string domain::get_location()
{
    return m_location;
}

std::string domain::get_404()
{
    return m_404;
}

wot::path domain::get_path(const std::string& path)
{
    return m_paths[path];
}

wot::path domain::find_wildcard_path(const std::string& path)
{
    wot::path result;

    for(auto& kv: m_paths)
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

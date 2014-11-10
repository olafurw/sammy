#include "cache_storage.hpp"

namespace sammy
{

cache::cache(const std::string& data, time_t expiration):
    m_data(data),
    m_expiration(expiration)
{
}

bool cache::is_expired() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_expiration < utils::current_time();
}

std::string cache::get_data() const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return m_data;
}

void cache::update(const std::string& data, time_t expiration)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_data = data;
    m_expiration = expiration;
}

cache_storage::cache_storage()
{
    m_log = std::unique_ptr<sammy::log>(new sammy::log());
}

bool cache_storage::is_expired(const std::string& domain, const std::string& path) const
{
    if(m_cache.count(domain) == 0)
    {
        return true;
    }
    else if(m_cache.at(domain).count(path) == 0)
    {
        return true;
    }

    return m_cache.at(domain).at(path)->is_expired();
}

std::string cache_storage::get_data(const std::string& domain, const std::string& path) const
{
    if(m_cache.count(domain) == 0)
    {
        return "";
    }
    else if(m_cache.at(domain).count(path) == 0)
    {
        return "";
    }

    m_log->debug("Fetch: " + domain + path);

    return m_cache.at(domain).at(path)->get_data();
}

void cache_storage::create_cache(const std::string& domain, const std::string& path, const std::string& data, time_t expiration)
{
    if(m_cache.count(domain) > 0 && m_cache[domain].count(path) > 0)
    {
        m_cache[domain][path]->update(data, expiration);
        return;
    }

    m_cache[domain][path] = std::unique_ptr<sammy::cache>(new sammy::cache(data, expiration));
}

void cache_storage::set(const std::string& domain, const std::string& path, const std::string& data, time_t expiration)
{
    if(m_cache.count(domain) == 0)
    {
        create_cache(domain, path, data, expiration);

        m_log->debug("Domain-cache-miss: " + domain + path);
        return;
    }
    else if(m_cache.at(domain).count(path) == 0)
    {
        create_cache(domain, path, data, expiration);

        m_log->debug("Cache-miss: " + domain + path);
        return;
    }

    m_cache[domain][path]->update(data, expiration);

    m_log->debug("Cache-update: " + domain + path);
}

}


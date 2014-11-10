#ifndef __SAMMY_CACHE__
#define __SAMMY_CACHE__

#include <mutex>
#include <map>
#include "log.hpp"
#include "utils/utils.hpp"

namespace sammy
{

class cache
{
public:
    cache(const std::string& data, time_t expiration);

    bool is_expired() const;
    std::string get_data() const;

    void update(const std::string& data, time_t expiration);

private:
    std::string m_data;
    time_t m_expiration;

    mutable std::mutex m_mutex;
};

class cache_storage
{
public:
    cache_storage();

    bool is_expired(const std::string& domain, const std::string& path) const;
    std::string get_data(const std::string& domain, const std::string& path) const;

    void set(const std::string& domain, const std::string& path, const std::string& data, time_t expiration);

private:
    void create_cache(const std::string& domain, const std::string& path, const std::string& data, time_t expiration);

    std::map<std::string, std::map<std::string, std::unique_ptr<cache>>> m_cache;
    std::unique_ptr<sammy::log> m_log;
};

}

#endif

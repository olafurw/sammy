#ifndef _SAMMY_SESSION_STORAGE_
#define _SAMMY_SESSION_STORAGE_

#include <map>
#include <memory>
#include <mutex>

namespace sammy
{
class session
{
public:
    session();
    ~session();

    std::string get_value(const std::string& key);
    void set_value(const std::string& key, const std::string& value);

private:
    std::map<std::string, std::string> m_session;
    std::mutex m_mutex;
};

class session_storage
{
public:
    session_storage();
    ~session_storage();

    void create_session(const std::string& id);
    void set_value(const std::string& id, const std::string& key, const std::string& value);
    std::string get_value(const std::string& id, const std::string& key);

private:
    std::map<std::string, std::unique_ptr<sammy::session>> m_storage;
};
}

#endif

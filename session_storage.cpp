#include "session_storage.hpp"

namespace sammy
{

session::session()
{
}

session::~session()
{
}

std::string session::get_value(const std::string& key) const
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if(m_session.count(key) == 0)
    {
        return "";
    }

    return m_session.at(key);
}

void session::set_value(const std::string& key, const std::string& value)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    m_session[key] = value;
}

session_storage::session_storage()
{
}

session_storage::~session_storage()
{
}

void session_storage::create_session(const std::string& id)
{
    if(m_storage.count(id) > 0)
    {
        return;
    }

    m_storage[id] = std::unique_ptr<sammy::session>(new sammy::session());
}

void session_storage::set_value(const std::string& id, const std::string& key, const std::string& value)
{
    if(m_storage.count(id) == 0)
    {
        create_session(id);
    }

    m_storage[id]->set_value(key, value);
}

std::string session_storage::get_value(const std::string& id, const std::string& key)
{
    if(m_storage.count(id) == 0)
    {
        create_session(id);
    }

    return m_storage[id]->get_value(key);
}

}

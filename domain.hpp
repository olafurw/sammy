#ifndef _WOT_DOMAIN_
#define _WOT_DOMAIN_

#include <map>
#include <vector>
#include <string>
#include <algorithm>

namespace wot
{
enum class path_type
{
    plain = 1,
    binary = 2,
    python = 3,
    unknown = 4
};

struct path
{
    std::string request;
    wot::path_type type;
    std::string file;

    path() { };
};

class domain
{
public:
    domain();

    void add_hostname(const std::string& hostname);
    void set_location(const std::string& location);
    void set_404(const std::string& file_404);
    void add_path(const wot::path& path);

    bool is_hostname(const std::string& hostname);
    std::string get_location();
    std::string get_404();
    wot::path get_path(const std::string& path);

private:
    std::vector<std::string> m_hostnames;
    std::string m_location;
    std::string m_404;
    std::map<std::string, wot::path> m_paths;
};
}

#endif

#ifndef _WOT_REQUEST_
#define _WOT_REQUEST_

#include <string>
#include <vector>
#include <memory>

#include "utils.hpp"
#include "log.hpp"

namespace wot
{
class request
{
public:
    request(const std::string& request);

    std::string get_host() const;
    std::string get_path() const;
    std::string get_method() const;

    bool errors() const;

private:
    int find_line_containing(const std::string& value, const std::vector<std::string>& request_lines) const;
    int find_line_starting_with(const std::string& value, const std::vector<std::string>& request_lines) const;

    std::string m_method;
    std::string m_path;
    std::string m_host;
    int m_error;

    std::unique_ptr<wot::log> m_log;
};
}

#endif

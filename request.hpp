#ifndef _WOT_REQUEST_
#define _WOT_REQUEST_

#include <string>
#include <vector>
#include <memory>

#include "utils/utils.hpp"

namespace wot
{
class request
{
public:
    request(const std::string& request);

    std::string get_host() const;
    std::string get_path() const;
    std::string get_method() const;
    std::string get_post_data() const;

    bool errors() const;

private:
    void parse_header(const std::string& header);
    void parse_host(const std::vector<std::string>& request_lines);
    void parse_cookies(const std::vector<std::string>& request_lines);
    void parse_post_data(const std::vector<std::string>& request_lines);

    std::string m_method;
    std::string m_path;
    std::string m_host;
    std::string m_post_data;
    int m_error;
};
}

#endif

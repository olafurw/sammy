#ifndef _WOT_REQUEST_
#define _WOT_REQUEST_

#include <string>
#include <vector>

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

private:
    int find_line_containing(const std::string& value, const std::vector<std::string>& request_lines) const;

    std::string method_;
    std::string path_;
    std::string host_;
    int error;
};
}

#endif

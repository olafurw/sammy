#ifndef _WOT_UTILITIES_
#define _WOT_UTILITIES_

#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <time.h>

namespace wot
{
namespace utils
{
    // Trimming
    // http://stackoverflow.com/a/217605/22459
    // Evan Teran
    std::string& ltrim(std::string& s);
    std::string& rtrim(std::string& s);
    std::string& trim(std::string& s);

    std::vector<std::string> split_string(const std::string& str, char token);

    // File to string
    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    // insane coder
    std::string file_to_string(const char* filename);
    
    std::string program_to_string(const std::string& run);
    std::unique_ptr<std::string> current_time(const char* format);

    bool ends_with(const std::string& str, const std::string& search);
    std::string mime_type(const std::string& filename);
}
}

#endif

#ifndef _WOT_UTILITIES_
#define _WOT_UTILITIES_

#include <unistd.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <memory>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>

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
    
    std::string sanitize_string(const std::string dirty);
    std::string program_to_string(const std::string& run, const std::string& argument = "");

    std::unique_ptr<std::string> current_time(const char* format);

    bool ends_with(const std::string& str, const std::string& search);
    std::string mime_type(const std::string& filename);
}
}

#endif

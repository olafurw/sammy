#ifndef _SAMMY_UTILITIES_
#define _SAMMY_UTILITIES_

#include <vector>
#include <fstream>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <memory>
#include <random>
#include <chrono>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

namespace sammy
{
namespace utils
{
    int random(const int from, const int to);

    // Trimming
    // http://stackoverflow.com/a/217605/22459
    // Evan Teran
    std::string& ltrim(std::string& s);
    std::string& rtrim(std::string& s);
    std::string& trim(std::string& s);

    std::string ltrim(const std::string& s);
    std::string rtrim(const std::string& s);
    std::string trim(const std::string& s);

    std::vector<std::string> split_string(const std::string& str, const char token, const bool skip_empty = false);
    
    // File modification time in unixtime
    time_t file_modified(const char* filename);

    // File to string
    // http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    // insane coder
    std::string file_to_string(const char* filename);
    std::vector<std::string> file_to_array(const char* filename, char token = '\n');
    
    std::string sanitize_string(const std::string dirty);
    std::string program_to_string(const std::string& run, const std::string& argument = "");

    std::string format_unixtime(time_t t, const char* format);
    std::unique_ptr<std::string> current_time(const char* format);
    time_t current_time();

    bool ends_with(const std::string& str, const std::string& search);
    int line_containing(const std::string& value, const std::vector<std::string>& request_lines);
    int line_starting_with(const std::string& value, const std::vector<std::string>& request_lines);

    std::string mime_type(const std::string& filename);

    // Base 64, modified version from
    // http://stackoverflow.com/a/6782480/22459
    // ryyst
    std::string base64_encode(std::string data);
    std::string base64_decode(std::string data);
}
}

#endif

#include "utils.hpp"

namespace wot
{
namespace utils
{
    std::string& ltrim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    std::string& rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    std::string& trim(std::string& s)
    {
        return ltrim(rtrim(s));
    }

    std::vector<std::string> split_string(const std::string& str, char token)
    {
        std::vector<std::string> segments;
        std::stringstream ss(str);
        std::string segment;
    
        while(std::getline(ss, segment, token))
        {
            segments.push_back(segment);
        }
    
        return segments;
    }

    std::string file_to_string(const char* filename)
    {
        std::ifstream in(filename, std::ios::in | std::ios::binary);
        std::string contents;
    
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
    
        in.read(&contents[0], contents.size());
        in.close();
    
        return contents;
    }
}
}

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

    std::string program_to_string(const std::string& run)
    {
        FILE* in;
        std::stringstream ss;

        if((in = popen(run.c_str(), "r")))
        {
            char buf[1024];

            while(fgets(buf, sizeof(buf), in) != NULL)
            {
                ss << buf;
            }

            pclose(in);
        }

        return ss.str();
    }
    
    std::unique_ptr<std::string> current_time(const char* format)
    {
        time_t rawtime;
        tm* timeinfo;
        char buffer[80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, format, timeinfo);
        std::unique_ptr<std::string> result{ new std::string(buffer) };

        return result;
    }

    bool ends_with(const std::string& str, const std::string& search)
    {
        unsigned int str_length = str.length();
        unsigned int search_length = search.length();

        if(str_length >= search_length)
        {
            return str.compare(str_length - search_length, search_length, search) == 0;
        }

        return false;
    }

    std::string mime_type(const std::string& filename)
    {
        if(ends_with(filename, ".html"))
        {
            return "text/html";
        }
        else if(ends_with(filename, ".css"))
        {
            return "text/css";
        }
        else if(ends_with(filename, ".js"))
        {
            return "application/javascript";
        }
        else if(ends_with(filename, ".png"))
        {
            return "image/png";
        }
        else if(ends_with(filename, ".jpg") || ends_with(filename, ".jpeg"))
        {
            return "image/jpeg";
        }

        return "text/plain";
    }
}
}


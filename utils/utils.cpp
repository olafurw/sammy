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
    
    std::string ltrim(const std::string& s)
    {
        std::string tmp_str = s;
        tmp_str.erase(tmp_str.begin(), std::find_if(tmp_str.begin(), tmp_str.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return tmp_str;
    }

    std::string rtrim(const std::string& s)
    {
        std::string tmp_str = s;
        tmp_str.erase(std::find_if(tmp_str.rbegin(), tmp_str.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), tmp_str.end());
        return tmp_str;
    }

    std::string trim(const std::string& s)
    {
        std::string tmp_str = s;
        return ltrim(rtrim(tmp_str));
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

    std::vector<std::string> file_to_array(const char* filename, char token)
    {
        return split_string(file_to_string(filename), token);
    }

    std::string sanitize_string(const std::string dirty)
    {
        static char whitelist[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_/";

        std::stringstream ss;

        for(const char& c : dirty)
        {
            if(strspn(&c, whitelist) != 0)
            {
                ss << c;
            }
            else
            {
                ss << "_";
            }
        }

        return ss.str();
    }

    std::string program_to_string(const std::string& run, const std::string& argument)
    {
        FILE* in;
        std::stringstream ss;
        std::string run_command = run + " " + argument;

        if((in = popen(run_command.c_str(), "r")))
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

    int line_containing(const std::string& value, const std::vector<std::string>& request_lines)
    {   
        for(unsigned int i = 0; i < request_lines.size(); ++i)
        {   
            if(request_lines.at(i).find(value) != std::string::npos)
            {
                return i;
            }   
        }   

        return -1; 
    }

    int line_starting_with(const std::string& value, const std::vector<std::string>& request_lines)
    {
        for(unsigned int i = 0; i < request_lines.size(); ++i)
        {
            if(request_lines.at(i).find(value) == 0)
            {
                return i;
            }
        }

        return -1;
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


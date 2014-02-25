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
            segments.emplace_back(std::move(segment));
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
        std::string run_command = run + " " + argument + " 2>&1";

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

    time_t current_time()
    {
        time_t rawtime;
        time(&rawtime);
        
        return rawtime;
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

    std::string base64_encode(std::string data)
    {
        static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                        '4', '5', '6', '7', '8', '9', '-', '_'};

        int input_length = data.length();
        int output_length = 4 * ((input_length + 2) / 3);

        std::stringstream output;

        for (int i = 0, j = 0; i < input_length;) {

            uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
            uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
            uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

            uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

            output << encoding_table[(triple >> 3 * 6) & 0x3F];
            output << encoding_table[(triple >> 2 * 6) & 0x3F];
            output << encoding_table[(triple >> 1 * 6) & 0x3F];
            output << encoding_table[(triple >> 0 * 6) & 0x3F];
        }

        if(input_length % 3 == 1)
        {
            output << "==";
        }
        else if(input_length % 3 == 2)
        {
            output << "=";
        }

        return output.str();
    }

    std::string base64_decode(std::string data)
    {
        static char encoding_table[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                        'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                        '4', '5', '6', '7', '8', '9', '-', '_'};
        
        static char* decoding_table = nullptr;
        if (decoding_table == nullptr)
        {
            decoding_table = static_cast<char*>(malloc(256));

            for (int i = 0; i < 64; i++)
            {
                decoding_table[(unsigned char) encoding_table[i]] = i;
            }
        }

        int input_length = data.length();
        int output_length = input_length / 4 * 3;

        if(data[input_length - 1] == '=')
        {
            output_length--;
        }
        if(data[input_length - 2] == '=')
        {
            output_length--;
        }

        std::stringstream output;

        for (int i = 0, j = 0; i < input_length;)
        {
            uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
            uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
            uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
            uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

            uint32_t triple = (sextet_a << 3 * 6)
            + (sextet_b << 2 * 6)
            + (sextet_c << 1 * 6)
            + (sextet_d << 0 * 6);

            if(j < output_length)
            {
                output << static_cast<char>((triple >> 2 * 8) & 0xFF);
                j++;
            }
            
            if(j < output_length)
            {
                output << static_cast<char>((triple >> 1 * 8) & 0xFF);
                j++;
            }

            if(j < output_length)
            {
                output << static_cast<char>((triple >> 0 * 8) & 0xFF);
                j++;
            }
        }

        return output.str();
    }
}
}


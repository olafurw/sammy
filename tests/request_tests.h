#include <cxxtest/TestSuite.h>
#include <vector>
#include <iostream>

#include "../utils/utils.hpp"
#include "../request.hpp"

class request_tests : public CxxTest::TestSuite
{
public:
    void test_post_with_data()
    {
        std::string req = R"(POST /enlighten/rest HTTP/1.1
Host: api.opencalais.com
Content-Type: application/x-www-form-urlencoded
Content-Length: length

licenseID=string&content=string&/paramsXML=string)";

        sammy::request r(req);

        TS_ASSERT_EQUALS("POST", r.get_method());
        TS_ASSERT_EQUALS("api.opencalais.com", r.get_host());
        TS_ASSERT_EQUALS("licenseID=string&content=string&/paramsXML=string", r.get_post_data());
    }

    void test_generic()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
Connection: close
User-Agent: Web-sniffer/1.0.47 (+http://web-sniffer.net/)
Accept-Encoding: gzip
Accept-Charset: ISO-8859-1,UTF-8;q=0.7,*;q=0.7
Cache-Control: no-cache
Accept-Language: de,en;q=0.7,en-us;q=0.3
Referer: http://web-sniffer.net/
)";

        sammy::request r(req);

        TS_ASSERT_EQUALS("GET", r.get_method());
        TS_ASSERT_EQUALS("www.google.com", r.get_host());
        TS_ASSERT_EQUALS("http://web-sniffer.net/", r.get_referer());
    }
};


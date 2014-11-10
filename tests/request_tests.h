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

        sammy::request r(1, req);

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

        sammy::request r(1, req);

        TS_ASSERT_EQUALS("GET", r.get_method());
        TS_ASSERT_EQUALS("www.google.com", r.get_host());
        TS_ASSERT_EQUALS("http://web-sniffer.net/", r.get_referer());
    }
    
    void test_if_modified_since()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(false, r.errors());

        TS_ASSERT_EQUALS("GET", r.get_method());
        TS_ASSERT_EQUALS("www.google.com", r.get_host());
        TS_ASSERT_EQUALS(783459811, r.get_if_modified_since());
    }
    
    void test_if_modified_since_error_day()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 32 Oct 1994 19:43:31 GMT
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }
    
    void test_if_modified_since_error_month()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 01 Xxx 1994 19:43:31 GMT
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }
    
    void test_if_modified_since_error_year()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 01 Oct 19X9 19:43:31 GMT
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }
    
    void test_if_modified_since_error_time()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 01 Oct 1999 19:43 GMT
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }
    
    void test_if_modified_since_error_gmt()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 01 Oct 1999 19:43:31 XXX
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }
    
    void test_if_modified_since_error_length()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Sat, 01 Oct 1999 19:43:31
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }
    
    void test_unknown_error()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
If-Modified-Since: Mon May  5 12:56:24 2014
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(false, r.errors());
        TS_ASSERT_EQUALS(1399294584, r.get_if_modified_since());
    }

    void test_port_default()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(false, r.errors());
        TS_ASSERT_EQUALS(80, r.get_port());
    }

    void test_port_8080()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com:8080
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(false, r.errors());
        TS_ASSERT_EQUALS(8080, r.get_port());
    }

    void test_port_error()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com:XXX
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
    }

    void test_identifier()
    {
        std::string req = R"(GET / HTTP/1.1
Host: www.google.com
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(false, r.errors());
        TS_ASSERT_EQUALS("6B86B273FF34FCE19D6B804E0C00000047ADA4EAA22F1D49C01E52DDB7875B4B", r.get_identifier());
    }

    void test_identifier_error()
    {
        std::string req = R"(GETA / HTTP/1.1
Host: www.google.com
)";

        sammy::request r(1, req);

        TS_ASSERT_EQUALS(true, r.errors());
        TS_ASSERT_EQUALS("", r.get_identifier());
    }
};


#include <cxxtest/TestSuite.h>
#include <vector>
#include <iostream>

#include "../utils/utils.hpp"
#include "../domain.hpp"

class domain_tests : public CxxTest::TestSuite
{
public:
    void test_add_hostname()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS(false, d.is_hostname("a"));
        d.add_hostname("a");
        TS_ASSERT_EQUALS(true, d.is_hostname("a"));
    }
    
    void test_add_empty_hostname()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS(false, d.is_hostname(""));
        d.add_hostname("");
        TS_ASSERT_EQUALS(false, d.is_hostname(""));
    }
    
    void test_add_existing_hostname()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS(false, d.is_hostname("abcd.com"));
        d.add_hostname("abcd.com");
        TS_ASSERT_EQUALS(true, d.is_hostname("abcd.com"));
        d.add_hostname("abcd.com");
        TS_ASSERT_EQUALS(true, d.is_hostname("abcd.com"));
    }
    
    void test_add_blacklist()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS(false, d.is_blacklisted("a"));
        d.add_blacklist("a");
        TS_ASSERT_EQUALS(true, d.is_blacklisted("a"));
    }
    
    void test_add_empty_blacklist()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS(false, d.is_blacklisted(""));
        d.add_blacklist("");
        TS_ASSERT_EQUALS(false, d.is_blacklisted(""));
    }
    
    void test_add_existing_blacklist()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS(false, d.is_blacklisted("123.123.123.123"));
        d.add_blacklist("123.123.123.123");
        TS_ASSERT_EQUALS(true, d.is_blacklisted("123.123.123.123"));
        d.add_blacklist("123.123.123.123");
        TS_ASSERT_EQUALS(true, d.is_blacklisted("123.123.123.123"));
    }
    
    void test_adding_location()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS("", d.get_location());
        d.set_location("/some/path/some_where");
        TS_ASSERT_EQUALS("/some/path/some_where", d.get_location());
    }
    
    void test_changing_location()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS("", d.get_location());
        d.set_location("/some/path/some_where");
        TS_ASSERT_EQUALS("/some/path/some_where", d.get_location());
        d.set_location("/some/other/path/some_where");
        TS_ASSERT_EQUALS("/some/other/path/some_where", d.get_location());
    }
    
    void test_adding_404()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS("", d.get_404());
        d.set_404("the_404_file.html");
        TS_ASSERT_EQUALS("the_404_file.html", d.get_404());
    }
    
    void test_changing_404()
    {
        sammy::domain d;
        
        TS_ASSERT_EQUALS("", d.get_404());
        d.set_404("the_404_file.html");
        TS_ASSERT_EQUALS("the_404_file.html", d.get_404());
        d.set_404("some_other_404_file.html");
        TS_ASSERT_EQUALS("some_other_404_file.html", d.get_404());
    }
    
    void test_add_path()
    {
        sammy::domain d;
        sammy::path p("a", sammy::path_type::plain, sammy::method_type::get, "file.html");
        
        d.add_path(p);
        
        sammy::path gp = d.get_path(sammy::method_type::get, "a");
        
        TS_ASSERT_EQUALS(gp, p);
    }

    void test_get_hostnames()
    {
        sammy::domain d;

        d.add_hostname("cznp.com");
        d.add_hostname("example.com ");
        d.add_hostname(" 127.0.0.1 ");
        d.add_hostname("example.com");

        std::vector<std::string> hostnames = d.get_hostnames();

        TS_ASSERT_EQUALS(3, hostnames.size());
        TS_ASSERT_EQUALS("cznp.com", hostnames[0]);
        TS_ASSERT_EQUALS("example.com", hostnames[1]);
        TS_ASSERT_EQUALS("127.0.0.1", hostnames[2]);
    }

};


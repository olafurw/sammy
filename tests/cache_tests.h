#include <cxxtest/TestSuite.h>
#include <vector>
#include <iostream>
#include <limits.h>

#include "../utils/utils.hpp"
#include "../cache_storage.hpp"

class cacge_tests : public CxxTest::TestSuite
{
public:
    void test_add_cache()
    {
        sammy::cache c("abc", 123);
        
        TS_ASSERT_EQUALS(true, c.is_expired())
        TS_ASSERT_EQUALS("abc", c.get_data());
    }

    void test_update_cache()
    {
        sammy::cache c("abc", 123);
        
        TS_ASSERT_EQUALS(true, c.is_expired())
        TS_ASSERT_EQUALS("abc", c.get_data());

        c.update("def", 345);

        TS_ASSERT_EQUALS(true, c.is_expired())
        TS_ASSERT_EQUALS("def", c.get_data());
    }

    void test_expired_cache()
    {
        sammy::cache c("abc", LONG_MAX);
        
        TS_ASSERT_EQUALS(false, c.is_expired())
        TS_ASSERT_EQUALS("abc", c.get_data());
    }

    void test_cache_storage()
    {
        sammy::cache_storage cs;
        
        cs.set("cznp.com", "/path", "abcd", 123);

        TS_ASSERT_EQUALS(true, cs.is_expired("cznp.com", "/path"))
        TS_ASSERT_EQUALS("abcd", cs.get_data("cznp.com", "/path"));
    }

    void test_cache_storage_multiple()
    {
        sammy::cache_storage cs;
        
        cs.set("cznp.com", "/path", "a", 123);
        cs.set("cznp.com", "/path/other", "b", 123);
        cs.set("google.se", "/path", "c", 123);
        cs.set("google.se", "/path/other", "d", 123);

        TS_ASSERT_EQUALS("a", cs.get_data("cznp.com", "/path"));
        TS_ASSERT_EQUALS("b", cs.get_data("cznp.com", "/path/other"));
        TS_ASSERT_EQUALS("c", cs.get_data("google.se", "/path"));
        TS_ASSERT_EQUALS("d", cs.get_data("google.se", "/path/other"));
    }

    void test_cache_storage_update_multiple()
    {
        sammy::cache_storage cs;
        
        cs.set("cznp.com", "/path", "a", 123);
        cs.set("cznp.com", "/path/other", "b", 123);
        cs.set("google.se", "/path", "c", 123);
        cs.set("google.se", "/path/other", "d", 123);

        TS_ASSERT_EQUALS("a", cs.get_data("cznp.com", "/path"));
        TS_ASSERT_EQUALS("b", cs.get_data("cznp.com", "/path/other"));
        TS_ASSERT_EQUALS("c", cs.get_data("google.se", "/path"));
        TS_ASSERT_EQUALS("d", cs.get_data("google.se", "/path/other"));

        cs.set("cznp.com", "/path", "e", 123);
        cs.set("cznp.com", "/path/other", "f", 123);
        cs.set("cznp.com", "/path/other/stuff", "f", 123);
        cs.set("google.se", "/path", "g", 123);
        cs.set("google.se", "/path/other", "h", 123);
        cs.set("google.se", "/path/other/stuff", "h", 123);

        TS_ASSERT_EQUALS("e", cs.get_data("cznp.com", "/path"));
        TS_ASSERT_EQUALS("f", cs.get_data("cznp.com", "/path/other"));
        TS_ASSERT_EQUALS("f", cs.get_data("cznp.com", "/path/other/stuff"));
        TS_ASSERT_EQUALS("g", cs.get_data("google.se", "/path"));
        TS_ASSERT_EQUALS("h", cs.get_data("google.se", "/path/other"));
        TS_ASSERT_EQUALS("h", cs.get_data("google.se", "/path/other/stuff"));
    }
};


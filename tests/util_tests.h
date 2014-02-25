#include <cxxtest/TestSuite.h>
#include <vector>
#include <iostream>

#include "../utils/utils.hpp"

class util_tests : public CxxTest::TestSuite
{
public:
    void test_trim()
    {
        std::vector<std::string> string_list { "a", "a ", " a", " a ", " - a - .\n " };
        std::vector<std::string> match_list { "a", "a", "a", "a", "- a - ." };

        for(unsigned int i = 0; i < string_list.size(); ++i)
        {
            TS_ASSERT_EQUALS(sammy::utils::trim(string_list.at(i)), match_list.at(i));
        }
    }

    void test_split_string()
    {
        std::string split{ ":a:b:cde:f:g::h:" };

        auto split_list = sammy::utils::split_string(split, ':');

        TS_ASSERT_EQUALS(8, split_list.size());

        TS_ASSERT_EQUALS("", split_list.at(0));
        TS_ASSERT_EQUALS("a", split_list.at(1));
        TS_ASSERT_EQUALS("b", split_list.at(2));
        TS_ASSERT_EQUALS("cde", split_list.at(3));
        TS_ASSERT_EQUALS("f", split_list.at(4));
        TS_ASSERT_EQUALS("g", split_list.at(5));
        TS_ASSERT_EQUALS("", split_list.at(6));
        TS_ASSERT_EQUALS("h", split_list.at(7));
    }
};


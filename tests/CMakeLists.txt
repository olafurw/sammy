INCLUDE_DIRECTORIES( . .. )

CXXTEST_ADD_TEST(util_tests util_tests.cpp ../utils/utils.cpp ${CMAKE_CURRENT_SOURCE_DIR}/util_tests.h)

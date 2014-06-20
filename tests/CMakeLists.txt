INCLUDE_DIRECTORIES( . .. )

CXXTEST_ADD_TEST(util_tests util_tests.cpp ../utils/utils.cpp ${CMAKE_CURRENT_SOURCE_DIR}/util_tests.h)
CXXTEST_ADD_TEST(request_tests request_tests.cpp ../utils/utils.cpp ../request.cpp ${CMAKE_CURRENT_SOURCE_DIR}/request_tests.h)
CXXTEST_ADD_TEST(domain_tests domain_tests.cpp ../utils/utils.cpp ../domain.cpp ${CMAKE_CURRENT_SOURCE_DIR}/domain_tests.h)

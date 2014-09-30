INCLUDE_DIRECTORIES( . .. )

CXXTEST_ADD_TEST(util_tests util_tests.cpp ../utils/utils.cpp ../utils/sha/sha256.cpp ${CMAKE_CURRENT_SOURCE_DIR}/util_tests.h)
CXXTEST_ADD_TEST(request_tests request_tests.cpp ../utils/utils.cpp ../utils/sha/sha256.cpp ../request.cpp ${CMAKE_CURRENT_SOURCE_DIR}/request_tests.h)
CXXTEST_ADD_TEST(domain_tests domain_tests.cpp ../utils/utils.cpp ../utils/sha/sha256.cpp ../domain.cpp ${CMAKE_CURRENT_SOURCE_DIR}/domain_tests.h)
CXXTEST_ADD_TEST(cache_tests cache_tests.cpp ../utils/utils.cpp ../utils/sha/sha256.cpp ../cache_storage.cpp ${CMAKE_CURRENT_SOURCE_DIR}/cache_tests.h)

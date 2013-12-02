cmake_minimum_required (VERSION 2.8)
project(wot)

set(CMAKE_CXX_FLAGS "-g -std=c++11")

add_executable(server main.cpp utils/utils.cpp log.cpp request.cpp response.cpp server.cpp config.cpp domain.cpp)

target_link_libraries(server pthread)

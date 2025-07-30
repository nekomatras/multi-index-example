cmake_minimum_required(VERSION 3.16)
project(multi-index-example)

set(CMAKE_CXX_STANDARD 17)

add_executable(main main.cpp)

target_include_directories(main PRIVATE /usr/include)

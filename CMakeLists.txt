cmake_minimum_required(VERSION 3.26)
project(a_star_algorythm)

set(CMAKE_CXX_STANDARD 17)

add_executable(a_star_algorythm main.cpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/grid.txt
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

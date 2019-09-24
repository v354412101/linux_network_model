cmake_minimum_required(VERSION 3.14)
project(net_test C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)

add_executable(net_test src/epoll_example.c)

#target_link_libraries(net_test pthread)
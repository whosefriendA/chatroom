cmake_minimum_required (VERSION 3.1)

project(client)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/include/hiredis)

find_library(HIREDIS_LIBRARY hiredis)

add_executable(client clientmain.cpp menu.cpp sign.cpp friend.cpp group.cpp)

target_link_libraries(client ${HIREDIS_LIBRARY})

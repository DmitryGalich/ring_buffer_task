cmake_minimum_required (VERSION 3.10)

project(ring_buffer_task)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")

add_executable(ring_buffer_task
                main.cpp)

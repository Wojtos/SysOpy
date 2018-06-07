cmake_minimum_required(VERSION 3.6)
project(zad1)

set(CMAKE_C_STANDARD 11)

add_executable(main main.c)
target_link_libraries(main pthread)
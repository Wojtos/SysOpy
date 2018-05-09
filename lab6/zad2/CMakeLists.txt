cmake_minimum_required(VERSION 3.6)
project(zad2)

set(CMAKE_C_STANDARD 11)

SET(GCC_COVERAGE_LINK_FLAGS    "-lrt")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

add_executable(server server.c)
add_executable(client client.c)
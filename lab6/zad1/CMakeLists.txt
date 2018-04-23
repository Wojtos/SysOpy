cmake_minimum_required(VERSION 3.6)
project(zad1)

set(CMAKE_C_STANDARD 11)

add_executable(server server.c)
add_executable(client client.c)
cmake_minimum_required(VERSION 3.6)
project(zad1)

set(CMAKE_C_STANDARD 11)

add_executable(barber barber.c)
add_executable(clients clients.c)

target_link_libraries(barber pthread)
target_link_libraries(barber rt)
target_link_libraries(clients pthread)
target_link_libraries(clients rt)


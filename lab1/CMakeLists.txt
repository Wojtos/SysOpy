cmake_minimum_required(VERSION 3.6)
project(lab1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -o")

add_library (sharedLibrary SHARED library.c)
add_library (staticLibrary STATIC library.c)
add_library (dynamicLibrary MODULE library.c)

add_executable(mainStatic main.c)
add_executable(mainShared main.c)

target_link_libraries(mainStatic staticLibrary)
target_link_libraries(mainShared sharedLibrary)
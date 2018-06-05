cmake_minimum_required(VERSION 3.6)
project(zad1)

set(CMAKE_C_STANDARD 11)

add_executable(clearTests clearTests.c)
add_executable(main main.c)
target_link_libraries(main pthread)
target_link_libraries(main m)


enable_testing()

add_test(clearing clearTests)

foreach(loop_var1 simple goodFilter)
    foreach(loop_var2 1 2 4 8)
        add_test(test${loop_var1}${loop_var2} ./main ${loop_var2} g.pgm test${loop_var1}${loop_var2}.pgm ${loop_var1})
    endforeach(loop_var2)
endforeach(loop_var1)
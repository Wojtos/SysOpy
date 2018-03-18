cmake_minimum_required(VERSION 3.6)
project(lab1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC ")

add_library (sharedLibrary SHARED library.c)
add_library (staticLibrary STATIC library.c)
add_library (dynamicLibrary MODULE library.c)

add_executable(mainStatic main.c)
add_executable(mainShared main.c)
add_executable(mainDynamic mainDynamic.c)
add_executable(clear clear.c)


target_link_libraries(mainStatic staticLibrary)
target_link_libraries(mainShared sharedLibrary)
target_link_libraries(mainDynamic ${CMAKE_DL_LIBS})

foreach(loop_var -O -O1 -O2 -O3 -Os)

    set(sharedName "sharedLibrary${loop_var}")
    set(staticName "staticLibrary${loop_var}")
    set(dynamicName "dynamicLibrary${loop_var}")

    add_library (${sharedName} SHARED library.c)
    add_library (${staticName} STATIC library.c)
    add_library (${dynamicName} MODULE library.c)

    target_compile_options(${sharedName} PUBLIC ${loop_var})
    target_compile_options(${staticName} PUBLIC ${loop_var})
    target_compile_options(${dynamicName} PUBLIC ${loop_var})


    set(mainShared "mainShared${loop_var}")
    set(mainStatic "mainStatic${loop_var}")
    set(mainDynamic "mainDynamic${loop_var}")

    add_executable(${mainStatic} main.c)
    add_executable(${mainShared} main.c)
    add_executable(${mainDynamic} mainDynamic.c)

    target_compile_options(${mainStatic} PUBLIC ${loop_var})
    target_compile_options(${mainShared} PUBLIC ${loop_var})
    target_compile_options(${mainDynamic} PUBLIC ${loop_var})

    target_link_libraries(${mainStatic} ${staticName})
    target_link_libraries(${mainShared} ${sharedName})
    target_link_libraries(${mainDynamic} ${CMAKE_DL_LIBS})
endforeach(loop_var)

enable_testing()

add_test(clearing clear)

add_test(test mainStatic ../raport2.txt test)

add_test(testStatic mainStatic ../raport3a.txt testStatic)
add_test(testShared mainShared ../raport3a.txt testShared)
add_test(testDynamic mainDynamic ../raport3a.txt testDynamic)

foreach(loop_var -O -O1 -O2 -O3 -Os)
    set(mainShared "mainShared${loop_var}")
    set(mainStatic "mainStatic${loop_var}")
    set(mainDynamic "mainDynamic${loop_var}")

    set(testShared "testShared${loop_var}")
    set(testStatic "testStatic${loop_var}")
    set(testDynamic "testDynamic${loop_var}")

    set(dynamicLibraryFile "./libdynamicLibrary${loop_var}.so")

    add_test(${testStatic} ${mainStatic} ../raport3b.txt ${testStatic})
    add_test(${testShared} ${mainShared} ../raport3b.txt ${testShared})
    add_test(${testDynamic} ${mainDynamic} ../raport3b.txt ${testDynamic} ${dynamicLibraryFile})
endforeach(loop_var)
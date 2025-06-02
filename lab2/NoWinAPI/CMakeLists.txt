cmake_minimum_required(VERSION 3.10)
project(NoWinAPI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab2
    main.cpp
    lab_functions.cpp
    globals.cpp
)

add_executable(lab2_tests
    tests/tests_lab2.cpp
    lab_functions.cpp
    globals.cpp
)
cmake_minimum_required(VERSION 3.29)
project(lab2)

set(CMAKE_CXX_STANDARD 14)

include_directories(NoWinAPI)
include_directories(WinAPI)

add_executable(lab2
    NoWinAPI/tests/tests_lab2.cpp
    NoWinAPI/globals.cpp
    NoWinAPI/globals.h
    NoWinAPI/lab_functions.cpp
    NoWinAPI/lab_functions.h
    NoWinAPI/main.cpp
    WinAPI/tests/tests_lab2.cpp
    WinAPI/lab_functions.cpp
    WinAPI/lab_functions.h
    WinAPI/main.cpp)

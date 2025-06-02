cmake_minimum_required(VERSION 3.23)
project(lab2 CXX)

add_executable(lab2 main.cpp lab_functions.cpp)
set_target_properties(lab2 PROPERTIES CXX_STANDARD 98)


add_executable(lab2_tests tests/tests_lab2.cpp lab_functions.cpp)
set_target_properties(lab2_tests PROPERTIES CXX_STANDARD 98)


enable_testing()
add_test(NAME Lab2Tests COMMAND lab2_tests)
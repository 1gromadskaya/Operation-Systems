cmake_minimum_required(VERSION 4.0) 
project(lab3 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)


add_library(marker_lib STATIC
    marker_logic.cpp
    marker_logic.h 
)
target_include_directories(marker_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(marker_lib INTERFACE Threads::Threads)


add_executable(lab3 main.cpp)

target_link_libraries(lab3 PRIVATE marker_lib)



enable_testing()

add_executable(runTests test_main.cpp)

target_link_libraries(runTests PRIVATE marker_lib GTest::gtest Threads::Threads)

add_test(NAME MarkerLibTests COMMAND runTests)

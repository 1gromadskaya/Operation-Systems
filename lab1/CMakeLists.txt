cmake_minimum_required(VERSION 3.10)
project(ProcessCreationLab)

# По умолчанию сборка производится с C++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

# Основные исполняемые файлы (C++98)
add_executable(Main Main.cpp)
add_executable(Creator Creator.cpp)
add_executable(Reporter Reporter.cpp)
add_executable(TestRunner TestRunner.cpp)

# Дополнительные цели для сборки с C++23 (на выбор)
add_executable(Main_CXX23 Main.cpp)
target_compile_features(Main_CXX23 PRIVATE cxx_std_23)

add_executable(Creator_CXX23 Creator.cpp)
target_compile_features(Creator_CXX23 PRIVATE cxx_std_23)

add_executable(Reporter_CXX23 Reporter.cpp)
target_compile_features(Reporter_CXX23 PRIVATE cxx_std_23)

add_executable(TestRunner_CXX23 TestRunner.cpp)
target_compile_features(TestRunner_CXX23 PRIVATE cxx_std_23)

enable_testing()
add_test(NAME RunTests COMMAND TestRunner)
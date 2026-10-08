cmake_minimum_required(VERSION 3.16)

project(lob)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CTest)

add_subdirectory("tests")
add_subdirectory("benchmarks")

add_library(LOBLib STATIC "src/json-parser.cpp" "src/symbol-repo.cpp" "src/memory-pool.cpp")

target_compile_options(LOBLib PRIVATE -fno-omit-frame-pointer)

target_include_directories(LOBLib PUBLIC "include/")

add_executable(MainApp "src/main.cpp")

target_link_libraries(MainApp PRIVATE LOBLib)

add_test(NAME "Tests" COMMAND tests)

cmake_minimum_required(VERSION 3.16)

project(lob)

set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(LOBLib STATIC "src/limit-order-book.cpp" "src/json-parser.cpp" "src/limit-order-book-handler.cpp" "src/symbol-repo.cpp" "src/memory-pool.cpp")

target_include_directories(LOBLib PUBLIC "include/")

add_executable(MainApp "src/main.cpp")

target_link_libraries(MainApp PRIVATE LOBLib)

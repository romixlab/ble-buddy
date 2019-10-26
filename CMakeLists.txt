cmake_minimum_required(VERSION 3.13)
project(blebuddy)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(blebuddy "")

add_subdirectory(src)
add_subdirectory(examples)

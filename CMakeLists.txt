cmake_minimum_required(VERSION 3.10)
set(APP_NAME "widgets")

project(${APP_NAME})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Include common CMake functions
include(src/CMakeLists.cmake)
include(resources/CMakeLists.cmake)
include(platforms/CMakeLists.cmake)

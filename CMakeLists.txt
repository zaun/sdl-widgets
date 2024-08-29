cmake_minimum_required(VERSION 3.10)
set(APP_NAME "widgets")

project(${APP_NAME})

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_STATIC            "Build static library build"                    OFF)
option(ENABLE_SHARED            "Build shared library build"                    ON)
option(ENABLE_DEMO              "Build demo application"                        ON)
option(ENABLE_EXTERNAL_STATIC   "Use static 3rd party libs (SDL, vorbis, etc.)" OFF)

# Include common CMake functions
include(src/CMakeLists.cmake)
include(resources/CMakeLists.cmake)
include(platforms/CMakeLists.cmake)

message(STATUS "")
message(STATUS "")
message(STATUS "SDL Widget Library options:")
message(STATUS "")
if (ENABLE_EXTERNAL_STATIC)
  message(STATUS "[X] Static 3rd-party Libraries    -DENABLE_EXTERNAL_STATIC=ON")
else()
  message(STATUS "[ ] Static 3rd-party Libraries    -DENABLE_EXTERNAL_STATIC=OFF")
endif()
if (ENABLE_STATIC)
  message(STATUS "[X] Static Library                -DENABLE_STATIC=ON")
else()
  message(STATUS "[ ] Static Library                -DENABLE_STATIC=OFF")
endif()
if (ENABLE_SHARED)
  message(STATUS "[X] Shared Library                -DENABLE_SHARED=ON")
else()
  message(STATUS "[ ] Shared Library                -DENABLE_SHARED=OFF")
endif()
if (ENABLE_DEMO)
  message(STATUS "[X] Demo Application              -DENABLE_DEMO=ON")
else()
  message(STATUS "[ ] Demo Application              -DENABLE_DEMO=OFF")
endif()

message(STATUS "")
message(STATUS "")

# get_cmake_property(_variableNames VARIABLES)

# foreach(_variableName ${_variableNames})
#     message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

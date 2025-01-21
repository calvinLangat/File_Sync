cmake_minimum_required(VERSION 3.16)
project(FileSync LANGUAGES CXX)

# Set C++ standard and enforce Debug mode by default
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type (Debug/Release)" FORCE)
endif()

# Add subdirectories for components
add_subdirectory(src/Utils)
add_subdirectory(src/Client)
add_subdirectory(src/Server)


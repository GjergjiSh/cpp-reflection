# Set the project name
project(reflection)

cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

# Include the Conan toolchain file
include(${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake)

# Set the CMAKE_MODULE_PATH variable
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/Release/generators)

# Find the nlohmann_json package
find_package(nlohmann_json REQUIRED)

# Add the library
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp" "include/*.h")
add_library(reflection ${LIB_SOURCES})

# Link the nlohmann_json library to the reflection library
target_link_libraries(reflection PUBLIC nlohmann_json::nlohmann_json)

include_directories(${CMAKE_SOURCE_DIR}/include ${nlohmann_json_INCLUDE_DIRS})

# Add the executable
add_executable(example example/main.cpp)

# Link the reflection library and the nlohmann_json library to the example executable
target_link_libraries(example PRIVATE reflection nlohmann_json::nlohmann_json)
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(reflection)

set(CMAKE_CXX_STANDARD 20)

# Add the library
file(GLOB_RECURSE LIB_SOURCES "src/*.cpp" "include/*.h")
add_library(reflection ${LIB_SOURCES})

# Include directories for the library
target_include_directories(reflection PUBLIC include)

# Add the executable
add_executable(example example/main.cpp)

# Link the library to the executable
target_link_libraries(example reflection)
cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GLFW (make sure you have installed it via Homebrew)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(include)

# Add the executable
add_executable(OpenGLProject main.cpp src/glad.c)

# Link the libraries
if(APPLE)
    target_link_libraries(OpenGLProject glfw "-framework OpenGL")
else()
    target_link_libraries(OpenGLProject glfw OpenGL)
endif()

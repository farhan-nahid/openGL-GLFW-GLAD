cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GLFW (installed via Homebrew)
find_package(glfw3 REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the executable
add_executable(OpenGLProject main.cpp src/glad.c)

# Link the libraries
if(APPLE)
    target_link_libraries(OpenGLProject glfw "-framework OpenGL" glu)
else()
    target_link_libraries(OpenGLProject glfw OpenGL::GL)
endif()

# Note: On Linux, you may need additional libraries
if(UNIX AND NOT APPLE)
    target_link_libraries(OpenGLProject X11 Xrandr Xi Xxf86vm Xinerama Xcursor rt m pthread)
endif()
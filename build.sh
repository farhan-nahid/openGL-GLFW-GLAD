#!/bin/bash

# Switch to script's directory
cd "$(dirname "$0")"

# Delete existing build directory if it exists, then create a new one
if [ -d "build" ]; then
    rm -rf build
fi
mkdir build
cd build

# Detect if we're on macOS
if [[ "$(uname)" == "Darwin" ]]; then
    echo "Building for macOS in $(pwd)..."

    # Clean any old build artifacts
    rm -f classroom

    # Build with explicit linking of all required frameworks
    clang++ -std=c++11 -DGL_SILENCE_DEPRECATION -Isrc \
        -o classroom ../classroom.cpp ../src/mac_fix.mm \
        -framework OpenGL -framework GLUT -framework Cocoa -framework Foundation -framework AppKit
else
    # On other platforms, just use the Makefile (assumes Makefile is in parent directory)
    cp ../Makefile .
    make
fi

if [ $? -eq 0 ]; then
    echo "Build successful! Run ./classroom to start the application from the build directory"
    cd build
    ./classroom
else
    echo "Build failed."
fi
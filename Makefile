CXX = clang++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Add source directories
SRC_DIR = src
INCLUDE_DIR = -I$(SRC_DIR)

# List all source files
SRC = classroom.cpp

# For macOS specific linking
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -DGL_SILENCE_DEPRECATION $(INCLUDE_DIR)
    LDFLAGS = -framework OpenGL -framework GLUT -framework Cocoa -framework Foundation -framework AppKit
    
    # Add the macOS specific file if it exists
    ifneq (,$(wildcard src/mac_fix.mm))
        SRC += src/mac_fix.mm
    endif
else
    CXXFLAGS += $(INCLUDE_DIR)
    LDFLAGS = -lGL -lGLU -lglut
endif

TARGET = classroom

all: $(TARGET)

$(TARGET): $(SRC)
    $(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
    rm -f $(TARGET)

.PHONY: all clean
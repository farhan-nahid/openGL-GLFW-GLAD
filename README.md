# 3D Classroom Simulator

The 3D Classroom Simulator is an interactive application designed to simulate a classroom environment using OpenGL. It features various classroom elements such as chairs, tables, cupboards, windows, and a fan, all rendered in 3D.

## Features

- **Interactive Camera**: Navigate through the classroom using keyboard and mouse controls for an immersive experience.
- **3D Models**: Includes detailed models of classroom furniture and appliances.
- **Realistic Lighting and Colors**: Enhanced visual appeal with realistic lighting and color schemes.
- **Interactive Fan**: Control the ceiling fan with a click.

## Directory Structure

- **`classroom.cpp`**: The main entry point of the application, handling rendering and user interaction.
- **`src/`**: Contains header files for different classroom elements:
  - `chair.hpp`: Defines the chair model.
  - `table.hpp`: Defines the table model.
  - `cupboard.hpp`: Defines the cupboard model.
  - `window.hpp`: Defines the window model.
  - `fan.hpp`: Defines the fan model.
  - `glut_compat.hpp`: Compatibility layer for GLUT functions across platforms.
  - `mac_fix.mm`: macOS-specific fixes for modern versions.

## Controls

- **Keyboard**:

  - `W`, `A`, `S`, `D`: Move the camera forward, left, backward, and right.
  - `Up`, `Left`, `Down`, `Right` arrows: Alternative controls for camera movement.
  - `+`, `-`: Increase or decrease movement speed.
  - `F`: Toggle ceiling fan on/off.
  - `R`: Reset camera position to starting point.
  - `C`: Toggle ceiling view (look straight up).
  - `Space`: Jump or elevate camera position.
  - `Shift`: Hold to move faster.
  - `Esc`: Exit the application.

- **Mouse**:
  - **Hold right mouse button** and move to change the camera angle.
  - **Left-click** on the fan switch to toggle the fan.
  - **Mouse wheel** to zoom in and out.

## Camera Control Tips

- Hold the right mouse button while moving the mouse to look around
- Combine keyboard and mouse for smooth navigation
- Use Shift+WASD for faster movement around the classroom

## Dependencies

- OpenGL
- GLUT/FreeGLUT

## Building and Running

### On macOS:

```bash
# Navigate to the project directory
cd /path/to/3D-classroom-simulator-main

# Build using the build script
./build.sh

# Run the application
./classroom
```

### On Linux:

```bash
# Navigate to the project directory
cd /path/to/3D-classroom-simulator-main

# Build using make
make

# Run the application
./classroom
```

### On Windows:

```bash
# Using MinGW/MSYS
g++ -o classroom classroom.cpp -lGL -lGLU -lfreeglut

# Run the application
classroom.exe
```

## Troubleshooting

If you encounter OpenGL deprecation warnings on macOS, these are expected as OpenGL is deprecated on macOS 10.14 (Mojave) and later. The application should still run correctly.

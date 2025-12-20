# Broken Arrow MeshEdit

A Blender-inspired mesh editor demo built with C++20, Win32, and DirectX 11. The UI ships with a dockable layout (tool shelf, outliner, properties, timeline) and a realtime 3D viewport featuring orbit/pan controls and simple primitive generation.

## Features

- Dockable panels that can be floated via the **Window** menu.
- DirectX 11 viewport with grid rendering and wireframe toggle.
- Primitive generation (Cube, Sphere, Grid) with adjustable sphere segments/rings.
- Blender-inspired layout and dark theme.

## Build (Visual Studio 2026)

1. Open a **Developer Command Prompt for VS 2026**.
2. Configure the project with CMake:
   ```bash
   cmake -S . -B build -G "Visual Studio 17 2022" -A x64
   ```
3. Build:
   ```bash
   cmake --build build --config Release
   ```
4. Run the produced executable from `build/Release/BrokenArrowMeshEdit.exe`.

## Controls

- **Left mouse drag**: Orbit camera.
- **Middle mouse drag**: Pan camera.
- **Mouse wheel**: Zoom.

Use the **Properties** panel to switch primitives and toggle wireframe or grid rendering.

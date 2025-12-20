# Broken Arrow MeshEdit

A Blender-inspired mesh editor prototype built in **C++20** with a DirectX 12 powered 3D viewport and a dockable, panel-based GUI. The project ships with both **CMake** and **Visual Studio Solution** build options.

## Features

- DirectX 12 rendered viewport with per-object materials and texture sampling.
- Dockable panel layout (Outliner, Properties, Sculpt panel) with resizable splitters.
- Mesh import/export via OBJ.
- Transform gizmos (translate/rotate/scale) via mouse + hotkeys.
- Sculpt brush with radius/strength sliders.
- Primitive mesh creation (cube, sphere, cylinder, plane).
- Editable material color and texture loading.

## Build with CMake (Windows)

```bash
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

The executable will be at:

```
build/Release/BrokenArrowMeshEdit.exe
```

## Build with Visual Studio Solution

1. Open `BrokenArrowMeshEdit.sln` in Visual Studio 2022.
2. Select **x64** and **Release** (or Debug).
3. Build and run the `BrokenArrowMeshEdit` project.

## Controls

- **Right Mouse Drag**: Pan camera.
- **Mouse Wheel**: Zoom camera.
- **W/E/R**: Switch gizmo (translate/rotate/scale).
- **S**: Toggle sculpt mode.
- **Left Mouse Drag**: Apply active gizmo or sculpt brush.

## Notes

- The viewport shader (`shaders/mesh.hlsl`) is compiled at runtime, so keep the shader path relative to the working directory.
- Texture loading uses WIC and supports PNG/JPEG files.

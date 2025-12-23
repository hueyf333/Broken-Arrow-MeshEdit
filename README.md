# Broken-Arrow-MeshEdit

**Status: ✅ Implementation Complete**

A Windows 10/11 mesh editor built with C++20, targeting Visual Studio 2022 (v143 toolset). Features OpenGL 4.5 rendering with GLFW, Dear ImGui interface, and ImGuizmo transform gizmos.

> **📋 See [IMPLEMENTATION_SUMMARY.md](IMPLEMENTATION_SUMMARY.md) for complete deliverables checklist**
> 
> **🚀 See [QUICKSTART.md](QUICKSTART.md) for getting started guide**
> 
> **🏗️ See [ARCHITECTURE.md](ARCHITECTURE.md) for technical details**

## Features

- **3D Viewport**: Offscreen framebuffer rendering with orbit/pan/zoom camera controls
- **Mesh Editing**: Vertex/Edge/Face selection modes with basic operations (extrude, subdivide, merge, delete)
- **UI Panels**: Dockable ImGui interface with Viewport, Outliner, Inspector, Console, and Metrics panels
- **Transform Gizmos**: ImGuizmo integration for visual manipulation of objects
- **File I/O**: Import/Export OBJ files, Save/Load JSON project files
- **Undo/Redo**: Command pattern implementation for transforms and mesh edits
- **Rendering**: Lambert lighting, wireframe/solid shading toggle, grid and axes overlay
- **Primitives**: Built-in creation of cube, plane, and icosphere meshes

## Quick Start

See [QUICKSTART.md](QUICKSTART.md) for a detailed getting started guide with examples and workflows.

## Requirements

- Windows 10/11
- Visual Studio 2022 or later
- CMake 3.20+
- vcpkg (latest)

## Building

### 1. Install vcpkg

If you don't have vcpkg installed:

```batch
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
```

Add vcpkg to your PATH or note its location.

### 2. Clone and Build

```batch
git clone https://github.com/hueyf333/Broken-Arrow-MeshEdit.git
cd Broken-Arrow-MeshEdit
```

### 3. Generate Visual Studio Solution

Set the vcpkg toolchain file path (adjust path as needed):

```batch
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64
```

Or if vcpkg is in the parent directory:

```batch
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 17 2022" -A x64
```

### 4. Build in Visual Studio

```batch
cmake --build build --config Release
```

Or open `build/MeshEditor.sln` in Visual Studio 2022 and build from the IDE.

### 5. Run

The executable will be in `build/Release/MeshEditor.exe`. Run it from that directory or copy the resources folder to the executable location.

```batch
cd build/Release
MeshEditor.exe
```

## Usage

### Keyboard Shortcuts

**Transform Tools:**
- `W` - Translate mode
- `E` - Rotate mode
- `R` - Scale mode
- `Q` - Select mode

**Selection Modes:**
- `1` - Vertex mode
- `2` - Edge mode
- `3` - Face mode

**View Controls:**
- `F` - Frame selected object
- `O` - Toggle Orthographic/Perspective camera
- `Z` - Toggle Wireframe/Solid shading
- `X` - Toggle grid visibility

**Editing:**
- `Delete` - Delete selected object
- `Ctrl+Z` - Undo
- `Ctrl+Y` - Redo

**Camera (in viewport):**
- `Alt+Left Mouse` - Orbit camera
- `Alt+Middle Mouse` - Pan camera
- `Alt+Right Mouse` - Zoom camera

### UI Panels

- **Viewport**: 3D view with transform gizmos
- **Outliner**: Scene hierarchy with object list
- **Inspector**: Object properties and selection info
- **Console**: Messages and operation log
- **Metrics**: FPS counter and shortcut reference

### Mesh Operations

Access via menus or shortcuts:
- **Subdivide**: Catmull-Clark style subdivision
- **Extrude Faces**: Extend selected faces along normal
- **Merge by Distance**: Combine nearby vertices
- **Recalculate Normals**: Rebuild mesh normals
- **Delete**: Remove selected vertices/edges/faces

### File Operations

- **Import OBJ**: File → Import OBJ (or `Ctrl+I`)
- **Export OBJ**: File → Export OBJ (or `Ctrl+E`)
- **Save Project**: File → Save Project (or `Ctrl+S`)
- **Open Project**: File → Open Project (or `Ctrl+O`)

## Project Structure

```
Broken-Arrow-MeshEdit/
├── CMakeLists.txt          # Build configuration
├── vcpkg.json              # Dependency manifest
├── src/                    # Source code
│   ├── main.cpp            # Entry point
│   ├── App.{hpp,cpp}       # Main application
│   ├── Core/               # Core systems
│   │   ├── Scene          # Scene graph
│   │   ├── Mesh           # Mesh data structures
│   │   ├── Camera         # Camera controller
│   │   ├── Selection      # Selection system
│   │   ├── Command        # Undo/redo
│   │   ├── MeshOps        # Mesh operations
│   │   └── Input          # Input handling
│   ├── Renderer/           # Rendering
│   │   ├── GLContext      # OpenGL context
│   │   ├── Shader         # Shader management
│   │   ├── MeshRenderer   # Mesh rendering
│   │   ├── GridRenderer   # Grid overlay
│   │   ├── AxisGizmoRenderer # Axis gizmo
│   │   └── FramebufferRenderer # Offscreen rendering
│   ├── UI/                 # User interface
│   │   ├── UIManager       # ImGui management
│   │   └── Panels/         # UI panels
│   └── IO/                 # File I/O
│       ├── OBJLoader       # OBJ import
│       ├── OBJWriter       # OBJ export
│       └── ProjectSerializer # JSON project files
├── resources/
│   ├── shaders/            # GLSL shaders
│   └── samples/            # Sample files
└── vendor/
    └── imguizmo/           # ImGuizmo library
```

## Dependencies

Managed via vcpkg manifest mode:

- **glfw3**: Window and input handling
- **glad**: OpenGL function loader (GL 4.5)
- **glm**: Mathematics library
- **imgui**: Immediate mode GUI with GLFW+OpenGL3 backends
- **tinyobjloader**: OBJ file parsing
- **nlohmann-json**: JSON serialization

ImGuizmo is vendored in `vendor/imguizmo/` as it's not available in vcpkg.

## License

See LICENSE file.

## Notes

- This is an MVP mesh editor focused on basic editing workflows
- Advanced features like PBR, physics, or animation are out of scope
- Tested on Windows 10/11 with Visual Studio 2022
- OpenGL 4.5 core profile required
- The application starts with a sample scene (cube and plane)
- All mesh operations work on the currently selected object
- Undo/redo is available for most operations
- ImGuizmo provides visual transform manipulation in the viewport

## Known Limitations

- Edge selection mode is partially implemented
- Bevel operation is stubbed out
- No file dialogs (files use hardcoded paths like "sample.obj", "export.obj", "project.json")
- Box/marquee selection not implemented
- Only single-level scene hierarchy (no parenting)
- Limited to simple diffuse/flat color materials

## Future Enhancements

Potential improvements not in current scope:
- Proper file dialogs for import/export
- More advanced subdivision surfaces
- UV mapping and texture support
- Multiple undo/redo stacks per object
- Plugin system for custom operations
- Scripting support (Lua/Python)

## Contributing

This is a learning project and demonstration of a mesh editing workflow. Feel free to fork and extend!


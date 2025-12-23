# Technical Architecture

## Overview

MeshEditor is a Windows mesh editing application built with modern C++20, using OpenGL 4.5 for rendering and Dear ImGui for the user interface. The architecture follows a modular design with clear separation of concerns.

## Core Systems

### 1. Application Layer (`App.hpp/cpp`)

The `App` class is the main application controller that:
- Manages the application lifecycle (init, update, render loop)
- Coordinates all subsystems
- Handles high-level input processing
- Manages the scene, camera, and selection state

**Key responsibilities:**
- Window and GL context initialization
- Main render loop
- Viewport raycast selection
- Keyboard shortcut dispatch

### 2. Rendering System (`src/Renderer/`)

#### GLContext
- GLFW window management
- OpenGL 4.5 core context creation
- Event callback setup

#### Shader
- GLSL shader compilation and linking
- Uniform value setting helpers
- Error reporting

#### MeshRenderer
- Renders scene meshes with Lambert lighting
- Supports wireframe/solid toggle
- Highlights selected objects

#### GridRenderer
- Draws floor grid for spatial reference
- Configurable size and divisions

#### AxisGizmoRenderer
- Renders RGB axis lines at origin
- Helps with orientation

#### FramebufferRenderer
- Offscreen rendering to texture
- Used for viewport panel in ImGui
- Handles resize events

### 3. Core Systems (`src/Core/`)

#### Scene & SceneObject
- Scene graph management
- Object transform hierarchy (position, rotation, scale)
- Object visibility and selection state
- Per-object material color

**Design:**
- `SceneObject` contains mesh data, transform, and visual properties
- `Scene` manages collection of objects
- Selection, duplication, hide/show operations

#### Mesh
- Indexed triangle mesh representation
- Vertex positions and normals
- GPU buffer management (VAO/VBO/EBO)
- Adjacency data for topology queries
- Primitive generators (cube, plane, icosphere)

**Key methods:**
- `uploadToGPU()`: Transfers mesh data to GPU buffers
- `calculateNormals()`: Computes smooth vertex normals
- `buildAdjacency()`: Builds vertex-to-face connectivity

#### Camera
- Orbit/pan/zoom controller
- Perspective and orthographic projection
- Ray generation for picking

**Features:**
- Yaw/pitch orbit around target point
- Distance-based zoom
- Frame-to-target functionality
- Ray casting for viewport selection

#### Selection
- Selection mode state (Object/Vertex/Edge/Face)
- Sets of selected elements per mode
- Mode switching and clearing

#### MeshOps
- Mesh editing operations namespace
- Operations: extrude, subdivide, merge, delete, recalc normals

**Key operations:**
- `extrudeFaces()`: Pushes selected faces along normal
- `subdivide()`: Catmull-Clark style subdivision
- `mergeByDistance()`: Welding close vertices
- `deleteFaces()`: Removes selected geometry

#### Raycast
- Ray-triangle intersection (Möller–Trumbore algorithm)
- Mesh picking in world space
- Closest vertex finding for vertex selection

#### Command & CommandHistory
- Undo/redo system using Command pattern
- `TransformCommand`: Captures object transform changes
- `MeshEditCommand`: Stores mesh snapshots before/after edits
- Stack-based history with execute/undo methods

#### Input
- Keyboard and mouse state tracking
- Key callback registration
- Mouse button down/pressed distinction
- Delta tracking for continuous input

### 4. User Interface (`src/UI/`)

#### UIManager
- ImGui context initialization
- Docking space setup
- Menu bar rendering
- Coordinates all panels

**Features:**
- Main menu with File/Edit/View/Tools/Help
- Status bar for operation hints
- Viewport texture pass-through

#### Panels

**ViewportPanel:**
- Displays offscreen framebuffer as ImGui image
- ImGuizmo overlay for transforms
- Tracks hover and focus state
- Mouse position tracking for raycast

**OutlinerPanel:**
- Scene hierarchy tree view
- Object selection and renaming
- Context menu for duplicate/hide/delete
- Add primitives menu

**InspectorPanel:**
- Object transform editing (position/rotation/scale)
- Material color picker
- Mesh statistics (vertex/triangle counts)
- Selection mode switcher

**ConsolePanel:**
- Scrollable message log
- Operation feedback
- Auto-scroll to bottom

**MetricsPanel:**
- FPS and frame time display
- Keyboard shortcuts reference

### 5. File I/O (`src/IO/`)

#### OBJLoader
- Wavefront OBJ import using tinyobjloader
- Triangulates faces automatically
- Generates normals if missing

#### OBJWriter
- OBJ export with vertices, normals, and faces
- Simple ASCII format

#### ProjectSerializer
- JSON project format using nlohmann-json
- Saves entire scene state
- Relative paths (future: asset references)

**JSON Structure:**
```json
{
  "version": "1.0",
  "objects": [
    {
      "name": "Object",
      "position": [x, y, z],
      "rotation": [x, y, z],
      "scale": [x, y, z],
      "color": [r, g, b],
      "visible": true,
      "mesh": { "vertices": [...], "indices": [...] }
    }
  ]
}
```

## Data Flow

### Rendering Pipeline

```
Scene (CPU)
  ↓
Mesh::uploadToGPU() → GPU Buffers (VAO/VBO/EBO)
  ↓
App::render() → FramebufferRenderer::bind()
  ↓
MeshRenderer::render(scene, camera, wireframe)
  ↓
Shader::use() + set uniforms
  ↓
glDrawElements() per object
  ↓
FramebufferRenderer::unbind()
  ↓
ViewportPanel::render() → displays texture
```

### Selection Pipeline

```
Mouse Click in Viewport
  ↓
App::performSelection()
  ↓
Convert screen coords to NDC
  ↓
Camera::getRayDirection() → world ray
  ↓
For each object:
  Raycast::castRay(ray, mesh, transform)
  ↓
  Möller–Trumbore per triangle
  ↓
Find closest hit
  ↓
Update Selection state
  ↓
UI updates (Inspector, status bar)
```

### Mesh Edit Pipeline

```
User triggers operation (menu/shortcut)
  ↓
Create MeshEditCommand
  ↓
MeshEditCommand::saveMeshState() (old mesh)
  ↓
MeshOps::operation(mesh, selection)
  ↓
Mesh::calculateNormals()
  ↓
Mesh::buildAdjacency()
  ↓
Mesh::uploadToGPU()
  ↓
CommandHistory::execute(command)
  ↓
Command stored for undo/redo
```

## Third-Party Libraries

| Library | Purpose | Integration |
|---------|---------|-------------|
| GLFW | Windowing, input | Direct API calls |
| GLAD | OpenGL loading | GL 4.5 core |
| GLM | Math (vectors, matrices) | Header-only |
| Dear ImGui | UI framework | GLFW+OpenGL3 backends |
| ImGuizmo | Transform gizmos | Vendored, direct include |
| tinyobjloader | OBJ parsing | Header-only |
| nlohmann-json | JSON serialization | Header-only |

## Build System

**vcpkg Manifest Mode:**
- `vcpkg.json`: Dependency specification
- `vcpkg-configuration.json`: Registry configuration
- All dependencies (except ImGuizmo) installed via vcpkg

**CMake:**
- Generates Visual Studio 2022 solution
- Finds packages via vcpkg toolchain file
- Copies resources to build directory post-build
- Sets startup project and working directory

## Extension Points

### Adding a New Mesh Operation

1. Add function to `MeshOps` namespace in `MeshOps.hpp/cpp`
2. Wire to menu in `UIManager::renderMenuBar()`
3. Optionally add keyboard shortcut in `App::setupShortcuts()`
4. Wrap in `MeshEditCommand` for undo support

### Adding a New Panel

1. Create `MyPanel.hpp/cpp` in `UI/Panels/`
2. Add `render(...)` method
3. Instantiate in `UIManager` constructor
4. Call `myPanel->render()` in `UIManager::renderUI()`

### Adding a New Primitive

1. Add static factory method to `Mesh` class
2. Generate vertices/indices/normals
3. Call `calculateNormals()` and `buildAdjacency()`
4. Wire to Outliner context menu in `OutlinerPanel`

## Performance Considerations

- **GPU Upload:** Mesh data uploaded once, not per frame
- **Raycasting:** CPU-based, O(n*triangles) per click
- **ImGui:** Immediate mode, minimal retained state
- **Framebuffer:** Resized only when viewport size changes
- **Selection:** Only active when clicking, not continuous

## Memory Management

- **Scene objects:** `std::unique_ptr` in `Scene`
- **UI panels:** `std::unique_ptr` in `UIManager`
- **Commands:** Stored in `CommandHistory` until cleared
- **Mesh data:** Manually freed with `freeGPU()` on removal
- **Shaders:** RAII in `Shader` class destructor

## Thread Safety

Current implementation is **single-threaded**:
- All operations on main thread
- No async I/O or background processing
- GLFW callbacks on main thread

Future multi-threading would require:
- Scene access synchronization
- Command queue for mesh operations
- Async file loading

## Debugging Tips

1. **Console Panel:** Check for error messages
2. **Mesh counts:** Inspector shows vertex/triangle count
3. **Status Bar:** Displays current operation
4. **Metrics Panel:** Shows FPS (performance indicator)
5. **Wireframe mode (Z):** Visualize mesh topology
6. **Shader errors:** Check console output on startup

## Code Style

- C++20 features used where appropriate
- RAII for resource management
- Namespace for related functions (`MeshOps`, `Raycast`, etc.)
- Smart pointers for ownership
- `const` correctness
- PascalCase for classes, camelCase for members

## Future Architecture Improvements

- **Entity Component System:** More flexible scene graph
- **Render graph:** Better rendering pipeline control
- **Job system:** Multi-threaded mesh operations
- **Plugin API:** Extensibility for custom tools
- **Scripting:** Lua/Python for automation
- **Asset management:** Proper resource loading/caching

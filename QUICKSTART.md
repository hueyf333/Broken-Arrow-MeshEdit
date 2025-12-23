# MeshEditor Quick Start Guide

## Installation & First Run

### 1. Prerequisites
- Windows 10/11
- Visual Studio 2022
- CMake 3.20+
- vcpkg installed

### 2. Setup vcpkg
```batch
# If you don't have vcpkg:
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
bootstrap-vcpkg.bat

# Set environment variable
set VCPKG_ROOT=C:\vcpkg
```

### 3. Build the Project
```batch
git clone https://github.com/hueyf333/Broken-Arrow-MeshEdit.git
cd Broken-Arrow-MeshEdit

# Option 1: Use the build script
build.bat

# Option 2: Manual CMake
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

### 4. Run
```batch
cd build\Release
MeshEditor.exe
```

## First Steps

### Understanding the Interface

When you launch MeshEditor, you'll see:

1. **Menu Bar** (top): File, Edit, View, Tools, Help
2. **Viewport** (center): 3D view with grid and axes
3. **Outliner** (left): List of objects in scene
4. **Inspector** (right): Properties of selected object
5. **Console** (bottom): Messages and operation feedback
6. **Metrics** (bottom-right): FPS and keyboard shortcuts
7. **Status Bar** (very bottom): Current operation hints

### Basic Workflow

#### Creating Objects

1. Right-click in the **Outliner** panel
2. Select **Add > Cube** (or Plane/Sphere)
3. The new object appears in the viewport

Or use the menu: **File > Add Object**

#### Selecting Objects

- **Click** on an object in the viewport
- Or click its name in the **Outliner**
- Selected objects are highlighted with orange tint

#### Moving Objects

1. Select an object
2. Use the **ImGuizmo** handles in the viewport:
   - Red arrow: Move along X axis
   - Green arrow: Move along Y axis
   - Blue arrow: Move along Z axis
3. Or edit position values in **Inspector** panel

#### Camera Controls

- **Alt + Left Mouse Drag**: Rotate around scene
- **Alt + Middle Mouse Drag**: Pan camera
- **Alt + Right Mouse Drag**: Zoom in/out
- **F** (with object selected): Frame selected object

#### Switching Views

- **O**: Toggle between Perspective and Orthographic camera
- **Z**: Toggle Wireframe/Solid shading
- **X**: Toggle grid on/off

## Example: Creating and Editing a Mesh

### Task: Create a subdivided cube

1. **Add a cube**
   - Right-click in Outliner > Add > Cube

2. **Select the cube**
   - Click on it in viewport or Outliner

3. **Subdivide it**
   - Menu: Tools > Subdivide
   - Notice the mesh now has more triangles (check Inspector)

4. **Change its color**
   - In Inspector panel, click the Color picker
   - Choose any color you like

5. **Frame it**
   - Press **F** to center camera on the cube

### Task: Edit a mesh in face mode

1. **Select your cube** (or create a new one)

2. **Switch to Face mode**
   - Press **3** key
   - Or use Inspector > Selection Mode > Face

3. **Select a face**
   - Click on any face of the cube in the viewport
   - Selected face count shows in Inspector

4. **Extrude the face**
   - Press **E** key
   - Or Menu: Tools > Extrude Faces
   - The face extends outward

5. **Switch back to object mode**
   - Press **1** for Vertex mode
   - Or click Selection Mode dropdown in Inspector

### Task: Import and export OBJ

1. **Export your edited cube**
   - File > Export OBJ (or **Ctrl+E**)
   - File saved as `export.obj`

2. **Import the sample**
   - File > Import OBJ (or **Ctrl+I**)
   - Loads `sample.obj` if present

## Common Operations

### Mesh Editing

| Operation | How To |
|-----------|--------|
| Subdivide | Tools > Subdivide |
| Extrude Faces | Select faces (mode 3), press **E** |
| Merge Vertices | Tools > Merge by Distance |
| Recalc Normals | Tools > Recalculate Normals |
| Delete Object | Select object, press **Delete** |

### Selection Modes

| Mode | Key | Description |
|------|-----|-------------|
| Vertex | **1** | Select individual vertices |
| Edge | **2** | Select edges (partial support) |
| Face | **3** | Select triangular faces |
| Object | Click in Outliner | Select entire objects |

### View Controls

| Action | Shortcut |
|--------|----------|
| Frame Selection | **F** |
| Ortho/Perspective | **O** |
| Wireframe Toggle | **Z** |
| Grid Toggle | **X** |

### Transform Shortcuts

| Tool | Key |
|------|-----|
| Translate | **W** |
| Rotate | **E** (or ImGuizmo) |
| Scale | **R** (or ImGuizmo) |
| Select | **Q** |

### Undo/Redo

- **Ctrl+Z**: Undo last operation
- **Ctrl+Y**: Redo

## Saving Your Work

### Save Project
```
File > Save Project (Ctrl+S)
```
Saves to `project.json` with all objects and transforms.

### Load Project
```
File > Open Project (Ctrl+O)
```
Loads `project.json` and recreates the scene.

## Tips and Tricks

1. **Use the Console** for operation feedback - it tells you what happened
2. **Check Metrics panel** for keyboard shortcuts reminder
3. **Use Status Bar** for hints about current tool
4. **Alt+Click** to orbit camera while working
5. **Frame Selection (F)** to quickly focus on an object
6. **Outliner right-click** for quick object operations (duplicate, hide, delete)

## Troubleshooting

### Viewport is black
- Make sure you have objects in the scene
- Try pressing **F** to frame an object
- Check if objects are visible in Outliner

### Can't select faces
- Make sure you're in Face mode (press **3**)
- Object must be selected first
- Click directly on a face in the viewport

### Camera is stuck
- Press **O** to toggle perspective/orthographic
- Try **Alt+Mouse** to reset camera orientation
- Select an object and press **F** to frame it

### Build fails
- Verify VCPKG_ROOT environment variable is set
- Make sure vcpkg is up to date: `git pull` in vcpkg directory
- Check that Visual Studio 2022 is installed
- Try deleting `build` folder and rebuilding

## Next Steps

- Experiment with combining primitives
- Try subdividing multiple times
- Export your creations as OBJ files
- Create complex shapes by extruding faces
- Save your projects and load them back

For more details, see the main [README.md](README.md).

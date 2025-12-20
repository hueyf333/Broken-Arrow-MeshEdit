// File: MeshEditor/README.md
# MeshEditor

MeshEditor is a Blender-inspired mesh editing prototype built with C++20, Qt 6 Widgets, and DirectX 11. It provides a dockable multi-panel layout, a live DX11 viewport, and a modular architecture for tools, import/export, and rendering.

## Features
- Dockable UI layout (Outliner, Properties, Content Browser, Console).
- DirectX 11 viewport with grid and mesh rendering.
- Scene graph with entities, transforms, selection, and materials.
- Tool framework with Select and Move gizmo support.
- Import: OBJ + glTF 2.0 (tinyobjloader / tinygltf).
- Export: OBJ (positions + faces).
- Basic PBR material UI placeholders.
- Undo/Redo skeleton for transforms and import.

## Usage
- **Orbit**: Alt + LMB drag
- **Pan**: Alt + MMB drag
- **Dolly**: Alt + RMB drag
- **Frame Selected**: F
- **Tools**: Select/Move/Rotate/Scale from toolbar
- **Import**: File → Import Mesh...

## Notes
- This is a minimal but runnable foundation for future sculpting, rigging, and UV tooling.
- Rendering uses runtime shader compilation; ensure `data/shaders` is copied next to the executable.

// File: MeshEditor/README.md
# MeshEditor

MeshEditor is a Blender-inspired mesh editor prototype built with **C++20**, **Qt 6 Widgets**, and **DirectX 11**. It provides a dockable UI, a real-time 3D viewport, and a lightweight scene system that can grow into more advanced tooling.

## Features
- Dockable multi-panel UI (Outliner, Properties, Viewport, Content, Console)
- DirectX 11 viewport embedded in a Qt widget
- Scene graph with entities, transforms, and material placeholders
- OBJ + glTF 2.0 import (positions, normals, UVs)
- OBJ export for selected entity
- Orbit/Pan/Dolly camera controls (Alt + mouse)
- Move gizmo (X-axis drag MVP), Rotate/Scale tools stubbed
- Simple PBR material UI placeholders (base color + texture path)
- Undo/Redo framework (import + transform changes)

## Controls
- **Alt + LMB**: Orbit
- **Alt + MMB**: Pan
- **Alt + RMB**: Dolly
- **F**: Frame selected
- **Left Click**: Select entity (AABB picking)

## Notes
- Shaders are compiled at runtime using `D3DCompileFromFile` and loaded from `data/shaders`.
- The renderer uses WIC to load textures (PNG/JPG).
- Additional tools (extrude/bevel, UV editor, sculpting) are stubbed with TODOs for future expansion.

// File: MeshEditor/README.md
# MeshEditor (Blender-Inspired)

MeshEditor is a Blender-inspired mesh editor prototype built with **C++20**, **Qt 6 Widgets**, and **DirectX 11**. It provides a dockable multi-panel interface, an interactive viewport, basic scene/selection management, and a tool framework that can expand into more advanced modeling workflows.

## Features
- Dockable UI panels: Outliner, Properties, Content Browser, Console.
- DirectX 11 viewport hosted in a Qt widget.
- Scene graph with entities, transforms, and selection.
- Import OBJ and glTF 2.0 (positions, normals, UV0, indices).
- Export selected mesh to OBJ.
- Basic material UI with base color and optional texture.
- Undo/Redo framework with command pattern.
- Tool framework (Select, Move, Rotate, Scale) and gizmo overlay.
- Grid/axes rendering, camera orbit/pan/dolly.

## Usage
1. Launch the app from Visual Studio.
2. Use **File > Import Mesh...** to load an OBJ or glTF model.
3. Select objects via the Outliner or click in the viewport.
4. Use toolbar tools (Select/Move/Rotate/Scale) and edit properties.
5. Use **File > Export Selected...** to export the current selection to OBJ.

## Notes
- The viewport uses DirectX 11 and runtime HLSL compilation.
- Texture loading uses WIC and supports PNG/JPG.
- Several advanced features are stubbed with TODO markers for future expansion.

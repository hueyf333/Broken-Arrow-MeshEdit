// File: MeshEditor/README_VS2026.md
# MeshEditor - Visual Studio 2026 Setup

1. Install Qt 6 (MSVC x64 build).
2. Set environment variable `QT6_DIR` (or `QTDIR`) to the Qt installation root.
3. Install vcpkg and enable manifest mode/integration.
4. Open `MeshEditor.sln` in Visual Studio 2026.
5. Build **x64** Debug or Release.
6. Run the app. The shader files in `data/shaders` are copied next to the executable on build.

## Notes
- The project uses C++20 and the vNext toolset.
- The DirectX 11 renderer uses `D3DCompileFromFile` at runtime, so the working directory must be the output folder.

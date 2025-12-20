// File: MeshEditor/README_VS2026.md
# Visual Studio 2026 Setup

## 1) Install Qt 6
- Install Qt 6 (Qt Widgets) for MSVC x64.
- Set environment variable **QT6_DIR** (or **QTDIR**) to your Qt install root, e.g.:
  - `C:\Qt\6.8.0\msvc2022_64`

## 2) Install vcpkg
- Install vcpkg and enable integration:
  - `vcpkg integrate install`
- Ensure manifest mode is enabled (this repo includes `vcpkg.json`).

## 3) Open Solution
- Open `MeshEditor.sln` in Visual Studio 2026.

## 4) Build
- Select **x64** and **Debug** or **Release**.
- Build the solution.

## 5) Run
- Run from Visual Studio.
- The post-build step copies shaders into `bin/<Config>/data/shaders`.
- The debugging working directory is set to the output folder so runtime shader compilation can locate the files.

## Troubleshooting
- If Qt DLLs are not found at runtime, ensure your PATH includes `$(QT6_DIR)\bin` (or `$(QTDIR)\bin`).
- If shaders fail to compile, verify the output directory contains `data/shaders`.

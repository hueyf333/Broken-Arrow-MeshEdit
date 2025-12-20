// File: MeshEditor/README_VS2026.md
# MeshEditor - Visual Studio 2026 Setup

Follow these steps to build and run the project in Visual Studio 2026.

## 1) Install Qt 6
Install Qt 6 (Qt Widgets + Qt Core + Qt GUI) for MSVC x64.

## 2) Set QT6_DIR (or QTDIR)
Set an environment variable to your Qt root folder:
- `QT6_DIR=C:\Qt\6.x.x\msvc2022_64`
- or `QTDIR=C:\Qt\6.x.x\msvc2022_64`

## 3) Install vcpkg and enable manifest mode
```powershell
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
```

## 4) Open the solution
Open `MeshEditor/MeshEditor.sln` in Visual Studio 2026.

## 5) Build
Select **Debug | x64** or **Release | x64** and build. The build will:
- Use vcpkg manifest mode (`vcpkg.json`) to install dependencies
- Copy `data/shaders` into the output directory

## 6) Run
Run the MeshEditor target. The working directory is set to the output folder, so shaders load from `./shaders`.

## Notes
- If Qt is installed in a custom location, update `QT6_DIR`/`QTDIR` before building.
- The project links against: `d3d11`, `dxgi`, `d3dcompiler`, `windowscodecs`, `Qt6Core`, `Qt6Gui`, `Qt6Widgets`.

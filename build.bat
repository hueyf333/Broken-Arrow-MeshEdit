@echo off
REM Build script for MeshEditor
REM Requires vcpkg to be installed and accessible

echo MeshEditor Build Script
echo =======================
echo.

REM Check if vcpkg toolchain path is provided
if "%VCPKG_ROOT%"=="" (
    echo ERROR: VCPKG_ROOT environment variable not set
    echo Please set VCPKG_ROOT to your vcpkg installation path
    echo Example: set VCPKG_ROOT=C:\vcpkg
    exit /b 1
)

set TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

if not exist "%TOOLCHAIN%" (
    echo ERROR: vcpkg toolchain not found at %TOOLCHAIN%
    echo Please verify your VCPKG_ROOT path
    exit /b 1
)

echo Using vcpkg toolchain: %TOOLCHAIN%
echo.

REM Configure
echo Configuring CMake...
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%" -G "Visual Studio 17 2022" -A x64

if errorlevel 1 (
    echo ERROR: CMake configuration failed
    exit /b 1
)

echo.
echo Configuration complete!
echo.
echo Building Release configuration...
cmake --build build --config Release

if errorlevel 1 (
    echo ERROR: Build failed
    exit /b 1
)

echo.
echo Build complete!
echo Executable: build\Release\MeshEditor.exe
echo.
echo To run: cd build\Release && MeshEditor.exe

cmake_minimum_required(VERSION 3.20)
project(BrokenArrowMeshEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(BrokenArrowMeshEdit WIN32
    src/main.cpp
    src/App.cpp
    src/Mesh.cpp
    src/MeshIO.cpp
    src/Primitives.cpp
    src/Renderer.cpp
    src/App.h
    src/Mesh.h
    src/MeshIO.h
    src/Primitives.h
    src/Renderer.h
    shaders/mesh.hlsl
)

target_include_directories(BrokenArrowMeshEdit PRIVATE src)

target_compile_definitions(BrokenArrowMeshEdit PRIVATE UNICODE _UNICODE)

target_link_libraries(BrokenArrowMeshEdit PRIVATE
    d3d12
    dxgi
    d3dcompiler
    windowscodecs
    comctl32
)

set_source_files_properties(shaders/mesh.hlsl PROPERTIES VS_SETTINGS "ExcludedFromBuild=true")

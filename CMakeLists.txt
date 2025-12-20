cmake_minimum_required(VERSION 3.23)
project(BrokenArrowMeshEdit LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(BrokenArrowMeshEdit WIN32
    src/main.cpp
)

target_compile_definitions(BrokenArrowMeshEdit PRIVATE UNICODE _UNICODE NOMINMAX)

if(MSVC)
    target_compile_options(BrokenArrowMeshEdit PRIVATE /W4 /permissive- /EHsc)
endif()

target_link_libraries(BrokenArrowMeshEdit
    d3d11
    dxgi
    d3dcompiler
    comctl32
    shcore
    gdi32
    user32
)

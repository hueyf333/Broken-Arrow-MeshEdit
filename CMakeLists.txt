cmake_minimum_required(VERSION 3.20)
project(MeshEditor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
    src/App.cpp
    src/Renderer/GLContext.cpp
    src/Renderer/MeshRenderer.cpp
    src/Renderer/GridRenderer.cpp
    src/Renderer/AxisGizmoRenderer.cpp
    src/Renderer/FramebufferRenderer.cpp
    src/Renderer/Shader.cpp
    src/UI/UIManager.cpp
    src/UI/Panels/ViewportPanel.cpp
    src/UI/Panels/OutlinerPanel.cpp
    src/UI/Panels/InspectorPanel.cpp
    src/UI/Panels/ConsolePanel.cpp
    src/UI/Panels/MetricsPanel.cpp
    src/Core/Scene.cpp
    src/Core/Mesh.cpp
    src/Core/MeshOps.cpp
    src/Core/Selection.cpp
    src/Core/Camera.cpp
    src/Core/Input.cpp
    src/Core/Command.cpp
    src/Core/Raycast.cpp
    src/IO/OBJLoader.cpp
    src/IO/OBJWriter.cpp
    src/IO/ProjectSerializer.cpp
)

set(HEADERS
    src/App.hpp
    src/Renderer/GLContext.hpp
    src/Renderer/MeshRenderer.hpp
    src/Renderer/GridRenderer.hpp
    src/Renderer/AxisGizmoRenderer.hpp
    src/Renderer/FramebufferRenderer.hpp
    src/Renderer/Shader.hpp
    src/UI/UIManager.hpp
    src/UI/Panels/ViewportPanel.hpp
    src/UI/Panels/OutlinerPanel.hpp
    src/UI/Panels/InspectorPanel.hpp
    src/UI/Panels/ConsolePanel.hpp
    src/UI/Panels/MetricsPanel.hpp
    src/Core/Scene.hpp
    src/Core/Mesh.hpp
    src/Core/MeshOps.hpp
    src/Core/Selection.hpp
    src/Core/Camera.hpp
    src/Core/Input.hpp
    src/Core/Command.hpp
    src/Core/Raycast.hpp
    src/IO/OBJLoader.hpp
    src/IO/OBJWriter.hpp
    src/IO/ProjectSerializer.hpp
)

# Vendor ImGuizmo (not in vcpkg)
set(IMGUIZMO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imguizmo)
file(GLOB IMGUIZMO_SOURCES ${IMGUIZMO_DIR}/*.cpp)
list(APPEND SOURCES ${IMGUIZMO_SOURCES})

# Executable
add_executable(MeshEditor ${SOURCES} ${HEADERS})

target_include_directories(MeshEditor PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${IMGUIZMO_DIR}
)

target_link_libraries(MeshEditor PRIVATE
    glfw
    glad::glad
    glm::glm
    imgui::imgui
    tinyobjloader::tinyobjloader
    nlohmann_json::nlohmann_json
)

# Copy resources to build directory
add_custom_command(TARGET MeshEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:MeshEditor>/resources
)

# Set Visual Studio startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MeshEditor)

# Platform-specific settings
if(MSVC)
    target_compile_options(MeshEditor PRIVATE /W4)
    set_target_properties(MeshEditor PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()

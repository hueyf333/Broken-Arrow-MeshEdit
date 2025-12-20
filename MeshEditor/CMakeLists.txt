// File: MeshEditor/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)
project(MeshEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(DEFINED ENV{QT6_DIR})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{QT6_DIR}")
elseif(DEFINED ENV{QTDIR})
  list(APPEND CMAKE_PREFIX_PATH "$ENV{QTDIR}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(MeshEditor
  src/app/Main.cpp
  src/app/App.cpp
  src/ui/MainWindow.cpp
  src/ui/Dock_Outliner.cpp
  src/ui/Dock_Properties.cpp
  src/ui/Dock_Content.cpp
  src/ui/Dock_Console.cpp
  src/ui/ViewportWidget.cpp
  src/ui/GizmoOverlay.cpp
  src/engine/Logger.cpp
  src/engine/Time.cpp
  src/engine/Command.cpp
  src/engine/UndoStack.cpp
  src/engine/FileSystem.cpp
  src/engine/Math.cpp
  src/scene/Scene.cpp
  src/scene/Entity.cpp
  src/scene/Components.cpp
  src/scene/Selection.cpp
  src/render/RendererDX11.cpp
  src/render/RenderMesh.cpp
  src/render/ShaderDX11.cpp
  src/render/Camera.cpp
  src/render/Grid.cpp
  src/render/TextureDX11.cpp
  src/render/WICTextureLoaderLite.cpp
  src/assets/Importer.cpp
  src/assets/Import_OBJ.cpp
  src/assets/Import_GLTF.cpp
  src/assets/Export_OBJ.cpp
  src/assets/AssetDatabase.cpp
  src/tools/Tool.cpp
  src/tools/ToolManager.cpp
  src/tools/Tool_Select.cpp
  src/tools/Tool_Transform.cpp
  src/tools/Tool_Extrude.cpp
  src/tools/Tool_Bevel.cpp
)

target_include_directories(MeshEditor PRIVATE src)

find_package(glm CONFIG REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(tinygltf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(MeshEditor PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets d3d11 dxgi d3dcompiler windowscodecs glm::glm tinyobjloader::tinyobjloader tinygltf nlohmann_json::nlohmann_json)

add_custom_command(TARGET MeshEditor POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/data/shaders
          $<TARGET_FILE_DIR:MeshEditor>
)

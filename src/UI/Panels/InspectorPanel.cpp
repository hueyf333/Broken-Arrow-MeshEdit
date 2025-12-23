#include "UI/Panels/InspectorPanel.hpp"
#include <imgui.h>

void InspectorPanel::render(Scene& scene, Selection& selection) {
    ImGui::Begin("Inspector");
    
    auto* obj = scene.getSelectedObject();
    if (obj) {
        ImGui::Text("Object: %s", obj->name.c_str());
        ImGui::Separator();
        
        ImGui::Text("Transform");
        ImGui::DragFloat3("Position", &obj->position.x, 0.1f);
        ImGui::DragFloat3("Rotation", &obj->rotation.x, 0.01f);
        ImGui::DragFloat3("Scale", &obj->scale.x, 0.01f);
        
        ImGui::Separator();
        ImGui::Text("Material");
        ImGui::ColorEdit3("Color", &obj->color.x);
        
        ImGui::Separator();
        ImGui::Text("Mesh Info");
        ImGui::Text("Vertices: %zu", obj->mesh.vertices.size());
        ImGui::Text("Triangles: %zu", obj->mesh.indices.size() / 3);
    } else {
        ImGui::Text("No object selected");
    }
    
    ImGui::Separator();
    ImGui::Text("Selection Mode");
    
    const char* modes[] = {"Object", "Vertex", "Edge", "Face"};
    int currentMode = static_cast<int>(selection.mode);
    if (ImGui::Combo("Mode", &currentMode, modes, 4)) {
        selection.setMode(static_cast<SelectionMode>(currentMode));
    }
    
    switch (selection.mode) {
        case SelectionMode::Vertex:
            ImGui::Text("Selected Vertices: %zu", selection.selectedVertices.size());
            break;
        case SelectionMode::Edge:
            ImGui::Text("Selected Edges: %zu", selection.selectedEdges.size());
            break;
        case SelectionMode::Face:
            ImGui::Text("Selected Faces: %zu", selection.selectedFaces.size());
            break;
        default:
            break;
    }
    
    ImGui::End();
}

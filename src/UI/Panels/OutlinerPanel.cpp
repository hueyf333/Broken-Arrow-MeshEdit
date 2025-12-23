#include "UI/Panels/OutlinerPanel.hpp"
#include <imgui.h>

void OutlinerPanel::render(Scene& scene) {
    ImGui::Begin("Outliner");
    
    auto& objects = scene.getObjects();
    for (size_t i = 0; i < objects.size(); ++i) {
        auto& obj = objects[i];
        
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        if (obj->selected) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        
        ImGui::TreeNodeEx((void*)(intptr_t)i, flags, "%s", obj->name.c_str());
        
        if (ImGui::IsItemClicked()) {
            scene.selectObject(static_cast<int>(i));
        }
        
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Duplicate")) {
                scene.duplicateObject(static_cast<int>(i));
            }
            if (ImGui::MenuItem("Hide")) {
                scene.hideObject(static_cast<int>(i));
            }
            if (ImGui::MenuItem("Delete")) {
                scene.removeObject(static_cast<int>(i));
            }
            ImGui::EndPopup();
        }
    }
    
    if (ImGui::BeginPopupContextWindow()) {
        if (ImGui::BeginMenu("Add")) {
            if (ImGui::MenuItem("Cube")) {
                scene.addObject("Cube", Mesh::createCube());
            }
            if (ImGui::MenuItem("Plane")) {
                scene.addObject("Plane", Mesh::createPlane());
            }
            if (ImGui::MenuItem("Sphere")) {
                scene.addObject("Sphere", Mesh::createIcosphere());
            }
            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }
    
    ImGui::End();
}

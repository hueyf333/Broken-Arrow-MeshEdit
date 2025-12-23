#include "UI/Panels/ViewportPanel.hpp"
#include <imgui.h>
#include <ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>

ViewportPanel::ViewportPanel()
    : m_texture(0), m_width(800), m_height(600), m_mouseX(0), m_mouseY(0), m_hovered(false), m_focused(false)
{
}

void ViewportPanel::render(Scene& scene, Camera& camera, Selection& selection) {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Viewport");
    
    m_hovered = ImGui::IsWindowHovered();
    m_focused = ImGui::IsWindowFocused();
    
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    m_width = viewportSize.x;
    m_height = viewportSize.y;
    
    if (m_texture != 0) {
        ImGui::Image((void*)(intptr_t)m_texture, viewportSize, ImVec2(0, 1), ImVec2(1, 0));
    }
    
    // ImGuizmo overlay
    if (selection.mode == SelectionMode::Object) {
        auto* obj = scene.getSelectedObject();
        if (obj) {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();
            
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 windowSize = ImGui::GetWindowSize();
            ImGuizmo::SetRect(windowPos.x, windowPos.y, windowSize.x, windowSize.y);
            
            glm::mat4 view = camera.getViewMatrix();
            glm::mat4 projection = camera.getProjectionMatrix();
            glm::mat4 transform = obj->getTransform();
            
            static ImGuizmo::OPERATION currentOp = ImGuizmo::TRANSLATE;
            
            ImGuizmo::Manipulate(
                glm::value_ptr(view),
                glm::value_ptr(projection),
                currentOp,
                ImGuizmo::LOCAL,
                glm::value_ptr(transform)
            );
        }
    }
    
    // Get mouse position relative to viewport
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    
    m_mouseX = mousePos.x - windowPos.x - contentMin.x;
    m_mouseY = mousePos.y - windowPos.y - contentMin.y;
    
    ImGui::End();
    ImGui::PopStyleVar();
}

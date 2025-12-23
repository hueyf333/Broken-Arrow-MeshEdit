#include "UI/Panels/MetricsPanel.hpp"
#include <imgui.h>

void MetricsPanel::render() {
    ImGui::Begin("Metrics");
    
    ImGui::Text("Application Metrics");
    ImGui::Separator();
    
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    
    ImGui::Separator();
    ImGui::Text("Shortcuts:");
    ImGui::BulletText("W/E/R - Translate/Rotate/Scale");
    ImGui::BulletText("Q - Select");
    ImGui::BulletText("1/2/3 - Vertex/Edge/Face mode");
    ImGui::BulletText("F - Frame selection");
    ImGui::BulletText("O - Toggle Ortho/Persp");
    ImGui::BulletText("Z/X - Cycle shading mode");
    ImGui::BulletText("Ctrl+Z/Y - Undo/Redo");
    
    ImGui::End();
}

#include "UI/Panels/ConsolePanel.hpp"
#include <imgui.h>

void ConsolePanel::render() {
    ImGui::Begin("Console");
    
    if (ImGui::Button("Clear")) {
        clear();
    }
    
    ImGui::Separator();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    
    for (const auto& msg : m_messages) {
        ImGui::TextUnformatted(msg.c_str());
    }
    
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
    ImGui::End();
}

void ConsolePanel::addMessage(const std::string& message) {
    m_messages.push_back(message);
}

void ConsolePanel::clear() {
    m_messages.clear();
}

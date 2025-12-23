#include "UI/UIManager.hpp"
#include "UI/Panels/ViewportPanel.hpp"
#include "UI/Panels/OutlinerPanel.hpp"
#include "UI/Panels/InspectorPanel.hpp"
#include "UI/Panels/ConsolePanel.hpp"
#include "UI/Panels/MetricsPanel.hpp"
#include "Core/Scene.hpp"
#include "Core/Command.hpp"
#include "Core/MeshOps.hpp"
#include "IO/OBJLoader.hpp"
#include "IO/OBJWriter.hpp"
#include "IO/ProjectSerializer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

UIManager::UIManager(GLFWwindow* window) : m_window(window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 450");
    
    m_viewportPanel = std::make_unique<ViewportPanel>();
    m_outlinerPanel = std::make_unique<OutlinerPanel>();
    m_inspectorPanel = std::make_unique<InspectorPanel>();
    m_consolePanel = std::make_unique<ConsolePanel>();
    m_metricsPanel = std::make_unique<MetricsPanel>();
    
    m_statusMessage = "Ready";
}

UIManager::~UIManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UIManager::endFrame() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UIManager::renderUI(Scene& scene, Camera& camera, Selection& selection, CommandHistory& history) {
    setupDockspace();
    renderMenuBar(scene, history);
    
    m_viewportPanel->render(scene, camera, selection);
    m_outlinerPanel->render(scene);
    m_inspectorPanel->render(scene, selection);
    m_consolePanel->render();
    m_metricsPanel->render();
    
    renderStatusBar();
}

bool UIManager::isViewportHovered() const {
    return m_viewportPanel->isHovered();
}

bool UIManager::isViewportFocused() const {
    return m_viewportPanel->isFocused();
}

void UIManager::getViewportSize(float& width, float& height) const {
    m_viewportPanel->getSize(width, height);
}

void UIManager::getViewportMousePos(float& x, float& y) const {
    m_viewportPanel->getMousePos(x, y);
}

void UIManager::addConsoleMessage(const std::string& message) {
    m_consolePanel->addMessage(message);
}

void UIManager::setStatusMessage(const std::string& message) {
    m_statusMessage = message;
}

void UIManager::setViewportTexture(GLuint texture) {
    m_viewportPanel->setTexture(texture);
}

void UIManager::setupDockspace() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);
    
    ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
}

void UIManager::renderMenuBar(Scene& scene, CommandHistory& history) {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                // Clear scene
            }
            if (ImGui::MenuItem("Open Project", "Ctrl+O")) {
                // TODO: File dialog
                ProjectSerializer::load("project.json", scene);
            }
            if (ImGui::MenuItem("Save Project", "Ctrl+S")) {
                ProjectSerializer::save("project.json", scene);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Import OBJ", "Ctrl+I")) {
                Mesh mesh;
                if (OBJLoader::load("sample.obj", mesh)) {
                    scene.addObject("Imported", mesh);
                }
            }
            if (ImGui::MenuItem("Export OBJ", "Ctrl+E")) {
                auto* obj = scene.getSelectedObject();
                if (obj) {
                    OBJWriter::save("export.obj", obj->mesh);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(m_window, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, history.canUndo())) {
                history.undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, history.canRedo())) {
                history.redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", "Delete")) {
                // Handle delete
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Frame Selection", "F")) {
                // Handled elsewhere
            }
            if (ImGui::MenuItem("Toggle Orthographic", "O")) {
                // Handled elsewhere
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            auto* obj = scene.getSelectedObject();
            bool hasSelection = obj != nullptr;
            
            if (ImGui::MenuItem("Subdivide", nullptr, false, hasSelection)) {
                if (obj) {
                    MeshOps::subdivide(obj->mesh);
                    obj->mesh.uploadToGPU();
                    m_consolePanel->addMessage("Subdivided mesh");
                }
            }
            if (ImGui::MenuItem("Extrude Faces", "E", false, hasSelection)) {
                // Will be triggered by shortcut
            }
            if (ImGui::MenuItem("Merge by Distance", nullptr, false, hasSelection)) {
                if (obj) {
                    MeshOps::mergeByDistance(obj->mesh, 0.001f);
                    obj->mesh.uploadToGPU();
                    m_consolePanel->addMessage("Merged vertices by distance");
                }
            }
            if (ImGui::MenuItem("Recalculate Normals", nullptr, false, hasSelection)) {
                if (obj) {
                    MeshOps::recalculateNormals(obj->mesh);
                    obj->mesh.uploadToGPU();
                    m_consolePanel->addMessage("Recalculated normals");
                }
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                m_consolePanel->addMessage("MeshEditor v0.1.0 - A simple mesh editing tool");
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
    
    ImGui::End();
}

void UIManager::renderStatusBar() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 statusPos = viewport->WorkPos;
    statusPos.y += viewport->WorkSize.y - 25.0f;
    ImGui::SetNextWindowPos(statusPos);
    ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 25.0f));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;
    
    ImGui::Begin("StatusBar", nullptr, flags);
    ImGui::Text("%s", m_statusMessage.c_str());
    ImGui::End();
}

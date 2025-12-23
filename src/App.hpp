#pragma once
#include "Renderer/GLContext.hpp"
#include "Renderer/MeshRenderer.hpp"
#include "Renderer/GridRenderer.hpp"
#include "Renderer/AxisGizmoRenderer.hpp"
#include "Renderer/FramebufferRenderer.hpp"
#include "UI/UIManager.hpp"
#include "Core/Scene.hpp"
#include "Core/Camera.hpp"
#include "Core/Selection.hpp"
#include "Core/Input.hpp"
#include "Core/Command.hpp"
#include <memory>

class App {
public:
    App();
    ~App();
    
    void run();
    
private:
    void init();
    void update();
    void render();
    void handleInput();
    void handleShortcuts();
    void setupShortcuts();
    void performSelection();
    bool isCtrlPressed() const;
    
    std::unique_ptr<GLContext> m_context;
    std::unique_ptr<MeshRenderer> m_meshRenderer;
    std::unique_ptr<GridRenderer> m_gridRenderer;
    std::unique_ptr<AxisGizmoRenderer> m_axisRenderer;
    std::unique_ptr<FramebufferRenderer> m_framebuffer;
    std::unique_ptr<UIManager> m_ui;
    
    Scene m_scene;
    Camera m_camera;
    Selection m_selection;
    Input m_input;
    CommandHistory m_commandHistory;
    
    bool m_wireframe;
    bool m_showGrid;
    bool m_showAxes;
    
    double m_lastMouseX;
    double m_lastMouseY;
    bool m_firstMouse;
};

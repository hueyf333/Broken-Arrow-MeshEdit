#include "App.hpp"
#include "Core/MeshOps.hpp"
#include "Core/Raycast.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <limits>

App::App()
    : m_wireframe(false)
    , m_showGrid(true)
    , m_showAxes(true)
    , m_lastMouseX(0.0)
    , m_lastMouseY(0.0)
    , m_firstMouse(true)
{
}

App::~App() {
}

void App::init() {
    m_context = std::make_unique<GLContext>(1600, 900, "MeshEditor");
    if (!m_context->getWindow()) {
        throw std::runtime_error("Failed to create window");
    }
    
    m_input.setWindow(m_context->getWindow());
    
    m_meshRenderer = std::make_unique<MeshRenderer>();
    m_gridRenderer = std::make_unique<GridRenderer>();
    m_axisRenderer = std::make_unique<AxisGizmoRenderer>();
    m_framebuffer = std::make_unique<FramebufferRenderer>();
    m_ui = std::make_unique<UIManager>(m_context->getWindow());
    
    // Create initial scene
    m_scene.addObject("Cube", Mesh::createCube());
    m_scene.addObject("Plane", Mesh::createPlane(10.0f, 10));
    m_scene.getObject(1)->position.y = -1.0f;
    m_scene.getObject(1)->color = glm::vec3(0.5f, 0.5f, 0.5f);
    
    m_ui->addConsoleMessage("MeshEditor initialized");
    m_ui->addConsoleMessage("Use W/E/R for transform tools, 1/2/3 for selection modes");
    
    // Setup keyboard shortcuts
    setupShortcuts();
}

void App::setupShortcuts() {
    // Transform tools (W/E/R only if not Ctrl pressed)
    m_input.registerKeyCallback(GLFW_KEY_W, [this]() {
        bool ctrlPressed = m_input.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || m_input.isKeyPressed(GLFW_KEY_RIGHT_CONTROL);
        if (!ctrlPressed) {
            m_ui->setStatusMessage("Translate mode");
        }
    });
    
    m_input.registerKeyCallback(GLFW_KEY_R, [this]() {
        bool ctrlPressed = m_input.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || m_input.isKeyPressed(GLFW_KEY_RIGHT_CONTROL);
        if (!ctrlPressed) {
            m_ui->setStatusMessage("Rotate mode (legacy - use ImGuizmo)");
        }
    });
    
    m_input.registerKeyCallback(GLFW_KEY_Q, [this]() {
        m_ui->setStatusMessage("Select mode");
    });
    
    // Selection modes
    m_input.registerKeyCallback(GLFW_KEY_1, [this]() {
        m_selection.setMode(SelectionMode::Vertex);
        m_ui->setStatusMessage("Vertex mode");
    });
    m_input.registerKeyCallback(GLFW_KEY_2, [this]() {
        m_selection.setMode(SelectionMode::Edge);
        m_ui->setStatusMessage("Edge mode");
    });
    m_input.registerKeyCallback(GLFW_KEY_3, [this]() {
        m_selection.setMode(SelectionMode::Face);
        m_ui->setStatusMessage("Face mode");
    });
    
    // View
    m_input.registerKeyCallback(GLFW_KEY_F, [this]() {
        auto* obj = m_scene.getSelectedObject();
        if (obj) {
            m_camera.frameTarget(obj->position, 3.0f);
            m_ui->setStatusMessage("Frame selection");
        }
    });
    
    m_input.registerKeyCallback(GLFW_KEY_O, [this]() {
        m_camera.toggleMode();
        m_ui->setStatusMessage("Toggle orthographic/perspective");
    });
    
    m_input.registerKeyCallback(GLFW_KEY_Z, [this]() {
        m_wireframe = !m_wireframe;
        m_ui->setStatusMessage(m_wireframe ? "Wireframe mode" : "Solid mode");
    });
    
    m_input.registerKeyCallback(GLFW_KEY_X, [this]() {
        m_showGrid = !m_showGrid;
        m_ui->setStatusMessage(m_showGrid ? "Grid on" : "Grid off");
    });
    
    // Mesh operations shortcuts
    m_input.registerKeyCallback(GLFW_KEY_E, [this]() {
        if (m_input.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || m_input.isKeyPressed(GLFW_KEY_RIGHT_CONTROL)) {
            // Ctrl+E is export
            return;
        }
        // E alone can be rotate or extrude - for now just show message
        if (m_selection.mode == SelectionMode::Face && m_selection.hasSelection()) {
            auto* obj = m_scene.getSelectedObject();
            if (obj) {
                MeshOps::extrudeFaces(obj->mesh, m_selection, 0.5f);
                obj->mesh.uploadToGPU();
                m_ui->setStatusMessage("Extruded faces");
                m_ui->addConsoleMessage("Extruded " + std::to_string(m_selection.selectedFaces.size()) + " faces");
            }
        }
    });
    
    m_input.registerKeyCallback(GLFW_KEY_S, [this]() {
        bool ctrlPressed = m_input.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || m_input.isKeyPressed(GLFW_KEY_RIGHT_CONTROL);
        bool shiftPressed = m_input.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || m_input.isKeyPressed(GLFW_KEY_RIGHT_SHIFT);
        
        if (shiftPressed) {
            m_ui->setStatusMessage("Toggle snapping (not implemented)");
        } else if (!ctrlPressed) {
            // S alone is scale
            m_ui->setStatusMessage("Scale mode");
        }
    });
    
    // Delete
    m_input.registerKeyCallback(GLFW_KEY_DELETE, [this]() {
        int idx = m_scene.getSelectedObjectIndex();
        if (idx >= 0) {
            m_scene.removeObject(idx);
            m_ui->setStatusMessage("Deleted object");
        }
    });
}

void App::run() {
    init();
    
    while (!m_context->shouldClose()) {
        update();
        render();
        m_context->swapBuffers();
        m_context->pollEvents();
    }
}

void App::update() {
    m_input.update();
    handleInput();
    handleShortcuts();
    
    // Update camera aspect ratio
    float vpWidth, vpHeight;
    m_ui->getViewportSize(vpWidth, vpHeight);
    if (vpWidth > 0 && vpHeight > 0) {
        m_camera.setAspect(vpWidth / vpHeight);
        m_framebuffer->resize(static_cast<int>(vpWidth), static_cast<int>(vpHeight));
    }
}

void App::render() {
    // Render to framebuffer
    m_framebuffer->bind();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (m_showGrid) {
        m_gridRenderer->render(m_camera);
    }
    
    if (m_showAxes) {
        m_axisRenderer->render(m_camera);
    }
    
    m_meshRenderer->render(m_scene, m_camera, m_wireframe);
    
    m_framebuffer->unbind();
    
    // Render UI
    int width, height;
    m_context->getFramebufferSize(width, height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    m_ui->beginFrame();
    m_ui->setViewportTexture(m_framebuffer->getTexture());
    m_ui->renderUI(m_scene, m_camera, m_selection, m_commandHistory);
    m_ui->endFrame();
}

void App::handleInput() {
    if (!m_ui->isViewportFocused()) {
        return;
    }
    
    // Camera controls
    bool altPressed = m_input.isKeyPressed(GLFW_KEY_LEFT_ALT) || m_input.isKeyPressed(GLFW_KEY_RIGHT_ALT);
    
    if (altPressed && m_input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        double dx, dy;
        m_input.getMouseDelta(dx, dy);
        m_camera.orbit(static_cast<float>(dx) * 0.5f, static_cast<float>(-dy) * 0.5f);
    }
    
    if (altPressed && m_input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE)) {
        double dx, dy;
        m_input.getMouseDelta(dx, dy);
        m_camera.pan(static_cast<float>(-dx), static_cast<float>(dy));
    }
    
    if (altPressed && m_input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {
        double dx, dy;
        m_input.getMouseDelta(dx, dy);
        m_camera.zoom(static_cast<float>(dy));
    }
    
    // Selection with left click (when not using Alt)
    if (!altPressed && m_input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        performSelection();
    }
}

void App::performSelection() {
    float mouseX, mouseY;
    m_ui->getViewportMousePos(mouseX, mouseY);
    
    float vpWidth, vpHeight;
    m_ui->getViewportSize(vpWidth, vpHeight);
    
    if (vpWidth <= 0 || vpHeight <= 0) return;
    
    // Convert to NDC
    float ndcX = (2.0f * mouseX) / vpWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mouseY) / vpHeight;
    
    glm::vec3 rayDir = m_camera.getRayDirection(ndcX, ndcY);
    glm::vec3 rayOrigin = m_camera.getPosition();
    
    if (m_selection.mode == SelectionMode::Object) {
        // Object selection
        float closestDist = std::numeric_limits<float>::max();
        int closestObj = -1;
        
        for (size_t i = 0; i < m_scene.getObjects().size(); ++i) {
            auto& obj = m_scene.getObjects()[i];
            if (!obj->visible) continue;
            
            auto hit = Raycast::castRay(rayOrigin, rayDir, obj->mesh, obj->getTransform(), static_cast<int>(i));
            if (hit && hit->distance < closestDist) {
                closestDist = hit->distance;
                closestObj = static_cast<int>(i);
            }
        }
        
        if (closestObj >= 0) {
            m_scene.selectObject(closestObj);
            m_ui->setStatusMessage("Selected: " + m_scene.getObject(closestObj)->name);
        }
    } else if (m_selection.mode == SelectionMode::Face) {
        // Face selection
        auto* obj = m_scene.getSelectedObject();
        if (obj) {
            auto hit = Raycast::castRay(rayOrigin, rayDir, obj->mesh, obj->getTransform(), 0);
            if (hit) {
                m_selection.selectedFaces.clear();
                m_selection.selectedFaces.insert(hit->triangleIndex);
                m_ui->setStatusMessage("Selected face: " + std::to_string(hit->triangleIndex));
            }
        }
    } else if (m_selection.mode == SelectionMode::Vertex) {
        // Vertex selection
        auto* obj = m_scene.getSelectedObject();
        if (obj) {
            auto hit = Raycast::castRay(rayOrigin, rayDir, obj->mesh, obj->getTransform(), 0);
            if (hit) {
                int closestVertex = Raycast::findClosestVertex(hit->hitPoint, obj->mesh, obj->getTransform(), 0.5f);
                if (closestVertex >= 0) {
                    m_selection.selectedVertices.clear();
                    m_selection.selectedVertices.insert(closestVertex);
                    m_ui->setStatusMessage("Selected vertex: " + std::to_string(closestVertex));
                }
            }
        }
    }
}

void App::handleShortcuts() {
    m_input.processKeyCallbacks();
    
    // Undo/Redo with Ctrl
    bool ctrlPressed = m_input.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || m_input.isKeyPressed(GLFW_KEY_RIGHT_CONTROL);
    
    if (ctrlPressed) {
        if (m_input.isKeyDown(GLFW_KEY_Z)) {
            m_commandHistory.undo();
            m_ui->setStatusMessage("Undo");
        }
        if (m_input.isKeyDown(GLFW_KEY_Y)) {
            m_commandHistory.redo();
            m_ui->setStatusMessage("Redo");
        }
    }
}

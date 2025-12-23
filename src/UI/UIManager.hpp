#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <vector>

// Forward declarations
class ViewportPanel;
class OutlinerPanel;
class InspectorPanel;
class ConsolePanel;
class MetricsPanel;
class Scene;
class Camera;
class Selection;
class CommandHistory;

class UIManager {
public:
    UIManager(GLFWwindow* window);
    ~UIManager();
    
    void beginFrame();
    void endFrame();
    
    void renderUI(Scene& scene, Camera& camera, Selection& selection, CommandHistory& history);
    
    bool isViewportHovered() const;
    bool isViewportFocused() const;
    void getViewportSize(float& width, float& height) const;
    void getViewportMousePos(float& x, float& y) const;
    
    void addConsoleMessage(const std::string& message);
    void setStatusMessage(const std::string& message);
    void setViewportTexture(GLuint texture);
    
private:
    GLFWwindow* m_window;
    
    std::unique_ptr<ViewportPanel> m_viewportPanel;
    std::unique_ptr<OutlinerPanel> m_outlinerPanel;
    std::unique_ptr<InspectorPanel> m_inspectorPanel;
    std::unique_ptr<ConsolePanel> m_consolePanel;
    std::unique_ptr<MetricsPanel> m_metricsPanel;
    
    std::string m_statusMessage;
    
    void setupDockspace();
    void renderMenuBar(Scene& scene, CommandHistory& history);
    void renderStatusBar();
};

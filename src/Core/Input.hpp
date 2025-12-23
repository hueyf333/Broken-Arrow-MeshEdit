#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <map>

class Input {
public:
    void setWindow(GLFWwindow* window) { m_window = window; }
    
    bool isKeyPressed(int key) const;
    bool isKeyDown(int key) const;
    bool isMouseButtonPressed(int button) const;
    
    void getMousePos(double& x, double& y) const;
    void getMouseDelta(double& dx, double& dy);
    
    void update();
    
    void registerKeyCallback(int key, std::function<void()> callback);
    void processKeyCallbacks();
    
private:
    GLFWwindow* m_window = nullptr;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    bool m_firstMouse = true;
    
    std::map<int, bool> m_keyStates;
    std::map<int, std::function<void()>> m_keyCallbacks;
};

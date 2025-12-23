#include "Core/Input.hpp"

bool Input::isKeyPressed(int key) const {
    if (!m_window) return false;
    return glfwGetKey(m_window, key) == GLFW_PRESS;
}

bool Input::isKeyDown(int key) const {
    if (!m_window) return false;
    int state = glfwGetKey(m_window, key);
    auto it = m_keyStates.find(key);
    bool wasPressed = (it != m_keyStates.end()) && it->second;
    return state == GLFW_PRESS && !wasPressed;
}

bool Input::isMouseButtonPressed(int button) const {
    if (!m_window) return false;
    return glfwGetMouseButton(m_window, button) == GLFW_PRESS;
}

void Input::getMousePos(double& x, double& y) const {
    if (!m_window) {
        x = y = 0.0;
        return;
    }
    glfwGetCursorPos(m_window, &x, &y);
}

void Input::getMouseDelta(double& dx, double& dy) {
    double x, y;
    getMousePos(x, y);
    
    if (m_firstMouse) {
        m_lastMouseX = x;
        m_lastMouseY = y;
        m_firstMouse = false;
    }
    
    dx = x - m_lastMouseX;
    dy = y - m_lastMouseY;
    
    m_lastMouseX = x;
    m_lastMouseY = y;
}

void Input::update() {
    if (!m_window) return;
    
    for (auto& pair : m_keyStates) {
        pair.second = (glfwGetKey(m_window, pair.first) == GLFW_PRESS);
    }
}

void Input::registerKeyCallback(int key, std::function<void()> callback) {
    m_keyCallbacks[key] = callback;
    m_keyStates[key] = false;
}

void Input::processKeyCallbacks() {
    for (const auto& pair : m_keyCallbacks) {
        if (isKeyDown(pair.first)) {
            pair.second();
        }
    }
}

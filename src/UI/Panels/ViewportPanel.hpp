#pragma once
#include "Core/Scene.hpp"
#include "Core/Camera.hpp"
#include "Core/Selection.hpp"
#include <glad/glad.h>

class ViewportPanel {
public:
    ViewportPanel();
    
    void render(Scene& scene, Camera& camera, Selection& selection);
    void setTexture(GLuint texture) { m_texture = texture; }
    
    bool isHovered() const { return m_hovered; }
    bool isFocused() const { return m_focused; }
    void getSize(float& width, float& height) const { width = m_width; height = m_height; }
    void getMousePos(float& x, float& y) const { x = m_mouseX; y = m_mouseY; }
    
private:
    GLuint m_texture;
    float m_width, m_height;
    float m_mouseX, m_mouseY;
    bool m_hovered;
    bool m_focused;
};

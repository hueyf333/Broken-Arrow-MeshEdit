#pragma once
#include "Renderer/Shader.hpp"
#include "Core/Camera.hpp"
#include <memory>
#include <glad/glad.h>

class AxisGizmoRenderer {
public:
    AxisGizmoRenderer();
    ~AxisGizmoRenderer();
    
    void render(const Camera& camera);
    
private:
    std::unique_ptr<Shader> m_shader;
    GLuint m_VAO, m_VBO;
    
    void buildAxes();
};

#include "Renderer/AxisGizmoRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

AxisGizmoRenderer::AxisGizmoRenderer() : m_VAO(0), m_VBO(0) {
    m_shader = std::make_unique<Shader>("resources/shaders/grid.vert", "resources/shaders/grid.frag");
    buildAxes();
}

AxisGizmoRenderer::~AxisGizmoRenderer() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void AxisGizmoRenderer::buildAxes() {
    std::vector<glm::vec3> vertices = {
        // X axis (red)
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f),
        // Y axis (green)
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 2.0f, 0.0f),
        // Z axis (blue)
        glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 2.0f)
    };
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void AxisGizmoRenderer::render(const Camera& camera) {
    m_shader->use();
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    m_shader->setMat4("uView", glm::value_ptr(view));
    m_shader->setMat4("uProjection", glm::value_ptr(projection));
    
    glLineWidth(3.0f);
    glBindVertexArray(m_VAO);
    
    // X axis - red
    m_shader->setVec3("uColor", 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    
    // Y axis - green
    m_shader->setVec3("uColor", 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);
    
    // Z axis - blue
    m_shader->setVec3("uColor", 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);
    
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

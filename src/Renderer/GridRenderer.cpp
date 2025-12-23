#include "Renderer/GridRenderer.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <vector>

GridRenderer::GridRenderer() : m_VAO(0), m_VBO(0), m_vertexCount(0) {
    m_shader = std::make_unique<Shader>("resources/shaders/grid.vert", "resources/shaders/grid.frag");
    buildGrid();
}

GridRenderer::~GridRenderer() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void GridRenderer::buildGrid() {
    std::vector<glm::vec3> vertices;
    float size = 20.0f;
    int divisions = 40;
    float step = size / divisions;
    float halfSize = size / 2.0f;
    
    // Lines parallel to X axis
    for (int i = 0; i <= divisions; ++i) {
        float z = -halfSize + i * step;
        vertices.push_back(glm::vec3(-halfSize, 0.0f, z));
        vertices.push_back(glm::vec3(halfSize, 0.0f, z));
    }
    
    // Lines parallel to Z axis
    for (int i = 0; i <= divisions; ++i) {
        float x = -halfSize + i * step;
        vertices.push_back(glm::vec3(x, 0.0f, -halfSize));
        vertices.push_back(glm::vec3(x, 0.0f, halfSize));
    }
    
    m_vertexCount = static_cast<int>(vertices.size());
    
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void GridRenderer::render(const Camera& camera) {
    m_shader->use();
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    m_shader->setMat4("uView", glm::value_ptr(view));
    m_shader->setMat4("uProjection", glm::value_ptr(projection));
    m_shader->setVec3("uColor", 0.3f, 0.3f, 0.3f);
    
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINES, 0, m_vertexCount);
    glBindVertexArray(0);
}

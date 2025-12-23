#include "Renderer/MeshRenderer.hpp"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

MeshRenderer::MeshRenderer()
    : m_lightDir(0.3f, -1.0f, 0.5f)
{
    m_shader = std::make_unique<Shader>("resources/shaders/mesh.vert", "resources/shaders/mesh.frag");
}

void MeshRenderer::render(const Scene& scene, const Camera& camera, bool wireframe) {
    m_shader->use();
    
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix();
    
    m_shader->setMat4("uView", glm::value_ptr(view));
    m_shader->setMat4("uProjection", glm::value_ptr(projection));
    m_shader->setVec3("uLightDir", m_lightDir.x, m_lightDir.y, m_lightDir.z);
    m_shader->setBool("uWireframe", wireframe);
    
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    for (const auto& obj : scene.getObjects()) {
        if (!obj->visible) continue;
        
        glm::mat4 model = obj->getTransform();
        m_shader->setMat4("uModel", glm::value_ptr(model));
        m_shader->setVec3("uColor", obj->color.x, obj->color.y, obj->color.z);
        m_shader->setBool("uSelected", obj->selected);
        
        glBindVertexArray(obj->mesh.VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(obj->mesh.indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

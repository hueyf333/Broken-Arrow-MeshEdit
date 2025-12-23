#pragma once
#include "Renderer/Shader.hpp"
#include "Core/Scene.hpp"
#include "Core/Camera.hpp"
#include <memory>

class MeshRenderer {
public:
    MeshRenderer();
    ~MeshRenderer() = default;
    
    void render(const Scene& scene, const Camera& camera, bool wireframe);
    
private:
    std::unique_ptr<Shader> m_shader;
    glm::vec3 m_lightDir;
};

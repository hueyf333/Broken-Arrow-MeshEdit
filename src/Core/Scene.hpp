#pragma once
#include "Core/Mesh.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <memory>

struct SceneObject {
    std::string name;
    Mesh mesh;
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler angles
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 color = glm::vec3(0.7f, 0.7f, 0.7f);
    bool visible = true;
    bool selected = false;
    
    glm::mat4 getTransform() const;
};

class Scene {
public:
    Scene();
    
    void addObject(const std::string& name, const Mesh& mesh);
    void removeObject(int index);
    SceneObject* getObject(int index);
    const std::vector<std::unique_ptr<SceneObject>>& getObjects() const { return m_objects; }
    std::vector<std::unique_ptr<SceneObject>>& getObjects() { return m_objects; }
    
    int getSelectedObjectIndex() const;
    SceneObject* getSelectedObject();
    void selectObject(int index);
    void deselectAll();
    
    void duplicateObject(int index);
    void hideObject(int index);
    void showObject(int index);
    void isolateObject(int index);
    
private:
    std::vector<std::unique_ptr<SceneObject>> m_objects;
};

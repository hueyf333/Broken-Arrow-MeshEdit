#include "Core/Scene.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 SceneObject::getTransform() const {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);
    return transform;
}

Scene::Scene() {
}

void Scene::addObject(const std::string& name, const Mesh& mesh) {
    auto obj = std::make_unique<SceneObject>();
    obj->name = name;
    obj->mesh = mesh;
    obj->mesh.uploadToGPU();
    m_objects.push_back(std::move(obj));
}

void Scene::removeObject(int index) {
    if (index >= 0 && index < static_cast<int>(m_objects.size())) {
        m_objects[index]->mesh.freeGPU();
        m_objects.erase(m_objects.begin() + index);
    }
}

SceneObject* Scene::getObject(int index) {
    if (index >= 0 && index < static_cast<int>(m_objects.size())) {
        return m_objects[index].get();
    }
    return nullptr;
}

int Scene::getSelectedObjectIndex() const {
    for (size_t i = 0; i < m_objects.size(); ++i) {
        if (m_objects[i]->selected) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

SceneObject* Scene::getSelectedObject() {
    int idx = getSelectedObjectIndex();
    return idx >= 0 ? m_objects[idx].get() : nullptr;
}

void Scene::selectObject(int index) {
    deselectAll();
    if (index >= 0 && index < static_cast<int>(m_objects.size())) {
        m_objects[index]->selected = true;
    }
}

void Scene::deselectAll() {
    for (auto& obj : m_objects) {
        obj->selected = false;
    }
}

void Scene::duplicateObject(int index) {
    if (index >= 0 && index < static_cast<int>(m_objects.size())) {
        auto& original = m_objects[index];
        auto duplicate = std::make_unique<SceneObject>();
        duplicate->name = original->name + " Copy";
        duplicate->mesh = original->mesh;
        duplicate->position = original->position + glm::vec3(1.0f, 0.0f, 0.0f);
        duplicate->rotation = original->rotation;
        duplicate->scale = original->scale;
        duplicate->color = original->color;
        duplicate->mesh.uploadToGPU();
        m_objects.push_back(std::move(duplicate));
    }
}

void Scene::hideObject(int index) {
    if (index >= 0 && index < static_cast<int>(m_objects.size())) {
        m_objects[index]->visible = false;
    }
}

void Scene::showObject(int index) {
    if (index >= 0 && index < static_cast<int>(m_objects.size())) {
        m_objects[index]->visible = true;
    }
}

void Scene::isolateObject(int index) {
    for (size_t i = 0; i < m_objects.size(); ++i) {
        m_objects[i]->visible = (static_cast<int>(i) == index);
    }
}

#include "IO/ProjectSerializer.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace ProjectSerializer {

bool save(const std::string& path, const Scene& scene) {
    json j;
    j["version"] = "1.0";
    j["objects"] = json::array();
    
    for (const auto& obj : scene.getObjects()) {
        json objJson;
        objJson["name"] = obj->name;
        objJson["position"] = {obj->position.x, obj->position.y, obj->position.z};
        objJson["rotation"] = {obj->rotation.x, obj->rotation.y, obj->rotation.z};
        objJson["scale"] = {obj->scale.x, obj->scale.y, obj->scale.z};
        objJson["color"] = {obj->color.x, obj->color.y, obj->color.z};
        objJson["visible"] = obj->visible;
        
        // Save mesh data
        json meshJson;
        meshJson["vertices"] = json::array();
        for (const auto& v : obj->mesh.vertices) {
            meshJson["vertices"].push_back({v.x, v.y, v.z});
        }
        meshJson["indices"] = obj->mesh.indices;
        
        objJson["mesh"] = meshJson;
        j["objects"].push_back(objJson);
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }
    
    file << j.dump(2);
    file.close();
    return true;
}

bool load(const std::string& path, Scene& scene) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << path << std::endl;
        return false;
    }
    
    json j;
    file >> j;
    file.close();
    
    // Clear existing scene
    while (!scene.getObjects().empty()) {
        scene.removeObject(0);
    }
    
    for (const auto& objJson : j["objects"]) {
        Mesh mesh;
        
        // Load vertices
        for (const auto& v : objJson["mesh"]["vertices"]) {
            mesh.vertices.push_back(glm::vec3(v[0], v[1], v[2]));
        }
        
        // Load indices
        mesh.indices = objJson["mesh"]["indices"].get<std::vector<unsigned int>>();
        
        mesh.calculateNormals();
        mesh.buildAdjacency();
        
        scene.addObject(objJson["name"], mesh);
        
        auto* obj = scene.getObject(static_cast<int>(scene.getObjects().size() - 1));
        if (obj) {
            obj->position = glm::vec3(objJson["position"][0], objJson["position"][1], objJson["position"][2]);
            obj->rotation = glm::vec3(objJson["rotation"][0], objJson["rotation"][1], objJson["rotation"][2]);
            obj->scale = glm::vec3(objJson["scale"][0], objJson["scale"][1], objJson["scale"][2]);
            obj->color = glm::vec3(objJson["color"][0], objJson["color"][1], objJson["color"][2]);
            obj->visible = objJson["visible"];
        }
    }
    
    return true;
}

} // namespace ProjectSerializer

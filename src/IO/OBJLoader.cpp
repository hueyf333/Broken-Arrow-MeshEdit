#include "IO/OBJLoader.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <iostream>

namespace OBJLoader {

bool load(const std::string& path, Mesh& mesh) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str());
    
    if (!warn.empty()) {
        std::cout << "OBJ Warning: " << warn << std::endl;
    }
    
    if (!err.empty()) {
        std::cerr << "OBJ Error: " << err << std::endl;
    }
    
    if (!ret) {
        return false;
    }
    
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.indices.clear();
    
    // Flatten all shapes into a single mesh
    for (const auto& shape : shapes) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            int fv = shape.mesh.num_face_vertices[f];
            
            // Only handle triangles
            if (fv != 3) {
                index_offset += fv;
                continue;
            }
            
            for (int v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                
                glm::vec3 vertex(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );
                
                mesh.vertices.push_back(vertex);
                mesh.indices.push_back(static_cast<unsigned int>(mesh.vertices.size() - 1));
            }
            
            index_offset += fv;
        }
    }
    
    mesh.calculateNormals();
    mesh.buildAdjacency();
    
    return true;
}

} // namespace OBJLoader

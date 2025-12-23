#include "IO/OBJWriter.hpp"
#include <fstream>
#include <iostream>

namespace OBJWriter {

bool save(const std::string& path, const Mesh& mesh) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << path << std::endl;
        return false;
    }
    
    file << "# OBJ exported from MeshEditor\n";
    file << "# Vertices: " << mesh.vertices.size() << "\n";
    file << "# Faces: " << (mesh.indices.size() / 3) << "\n\n";
    
    // Write vertices
    for (const auto& v : mesh.vertices) {
        file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }
    
    file << "\n";
    
    // Write normals
    for (const auto& n : mesh.normals) {
        file << "vn " << n.x << " " << n.y << " " << n.z << "\n";
    }
    
    file << "\n";
    
    // Write faces
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        unsigned int i0 = mesh.indices[i] + 1;
        unsigned int i1 = mesh.indices[i + 1] + 1;
        unsigned int i2 = mesh.indices[i + 2] + 1;
        file << "f " << i0 << "//" << i0 << " " 
             << i1 << "//" << i1 << " " 
             << i2 << "//" << i2 << "\n";
    }
    
    file.close();
    return true;
}

} // namespace OBJWriter

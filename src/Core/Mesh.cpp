#include "Core/Mesh.hpp"
#include <glad/glad.h>
#include <cmath>
#include <map>

void Mesh::uploadToGPU() {
    if (VAO != 0) freeGPU();
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    glBindVertexArray(0);
}

void Mesh::freeGPU() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (NBO) glDeleteBuffers(1, &NBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    VAO = VBO = NBO = EBO = 0;
}

void Mesh::calculateNormals() {
    normals.clear();
    normals.resize(vertices.size(), glm::vec3(0.0f));
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];
        
        glm::vec3 v0 = vertices[i0];
        glm::vec3 v1 = vertices[i1];
        glm::vec3 v2 = vertices[i2];
        
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        
        normals[i0] += normal;
        normals[i1] += normal;
        normals[i2] += normal;
    }
    
    for (auto& n : normals) {
        n = glm::normalize(n);
    }
}

void Mesh::buildAdjacency() {
    vertexToFaces.clear();
    vertexToFaces.resize(vertices.size());
    
    for (size_t i = 0; i < indices.size(); i += 3) {
        int faceIdx = static_cast<int>(i / 3);
        vertexToFaces[indices[i]].push_back(faceIdx);
        vertexToFaces[indices[i + 1]].push_back(faceIdx);
        vertexToFaces[indices[i + 2]].push_back(faceIdx);
    }
}

Mesh Mesh::createCube(float size) {
    Mesh mesh;
    float s = size / 2.0f;
    
    mesh.vertices = {
        {-s, -s, -s}, { s, -s, -s}, { s,  s, -s}, {-s,  s, -s}, // back
        {-s, -s,  s}, { s, -s,  s}, { s,  s,  s}, {-s,  s,  s}, // front
    };
    
    mesh.indices = {
        0, 1, 2, 2, 3, 0, // back
        4, 6, 5, 6, 4, 7, // front
        0, 4, 7, 7, 3, 0, // left
        1, 5, 6, 6, 2, 1, // right
        3, 7, 6, 6, 2, 3, // top
        0, 1, 5, 5, 4, 0  // bottom
    };
    
    mesh.calculateNormals();
    mesh.buildAdjacency();
    return mesh;
}

Mesh Mesh::createPlane(float size, int subdivisions) {
    Mesh mesh;
    float step = size / subdivisions;
    float halfSize = size / 2.0f;
    
    for (int z = 0; z <= subdivisions; ++z) {
        for (int x = 0; x <= subdivisions; ++x) {
            float xPos = -halfSize + x * step;
            float zPos = -halfSize + z * step;
            mesh.vertices.push_back({xPos, 0.0f, zPos});
            mesh.normals.push_back({0.0f, 1.0f, 0.0f});
        }
    }
    
    for (int z = 0; z < subdivisions; ++z) {
        for (int x = 0; x < subdivisions; ++x) {
            int topLeft = z * (subdivisions + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (subdivisions + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);
            
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
        }
    }
    
    mesh.buildAdjacency();
    return mesh;
}

Mesh Mesh::createIcosphere(float radius, int subdivisions) {
    Mesh mesh;
    
    // Create icosahedron
    const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
    
    mesh.vertices = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1}
    };
    
    for (auto& v : mesh.vertices) {
        v = glm::normalize(v) * radius;
    }
    
    mesh.indices = {
        0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
        1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
        3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
        4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1
    };
    
    // Simple subdivision (just normalize midpoints)
    for (int i = 0; i < subdivisions; ++i) {
        std::vector<unsigned int> newIndices;
        std::map<std::pair<unsigned int, unsigned int>, unsigned int> midpointCache;
        
        auto getMidpoint = [&](unsigned int i1, unsigned int i2) {
            auto key = std::make_pair(std::min(i1, i2), std::max(i1, i2));
            auto it = midpointCache.find(key);
            if (it != midpointCache.end()) {
                return it->second;
            }
            glm::vec3 mid = (mesh.vertices[i1] + mesh.vertices[i2]) * 0.5f;
            mid = glm::normalize(mid) * radius;
            unsigned int idx = static_cast<unsigned int>(mesh.vertices.size());
            mesh.vertices.push_back(mid);
            midpointCache[key] = idx;
            return idx;
        };
        
        for (size_t j = 0; j < mesh.indices.size(); j += 3) {
            unsigned int v0 = mesh.indices[j];
            unsigned int v1 = mesh.indices[j + 1];
            unsigned int v2 = mesh.indices[j + 2];
            
            unsigned int m01 = getMidpoint(v0, v1);
            unsigned int m12 = getMidpoint(v1, v2);
            unsigned int m20 = getMidpoint(v2, v0);
            
            newIndices.insert(newIndices.end(), {v0, m01, m20});
            newIndices.insert(newIndices.end(), {v1, m12, m01});
            newIndices.insert(newIndices.end(), {v2, m20, m12});
            newIndices.insert(newIndices.end(), {m01, m12, m20});
        }
        
        mesh.indices = newIndices;
    }
    
    mesh.calculateNormals();
    mesh.buildAdjacency();
    return mesh;
}

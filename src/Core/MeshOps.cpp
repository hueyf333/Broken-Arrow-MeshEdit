#include "Core/MeshOps.hpp"
#include <map>
#include <algorithm>

namespace MeshOps {

void extrudeFaces(Mesh& mesh, const Selection& selection, float distance) {
    if (selection.selectedFaces.empty()) return;
    
    std::map<int, int> oldToNew;
    std::vector<glm::vec3> newVertices;
    std::vector<unsigned int> newIndices;
    
    // Copy existing vertices
    for (const auto& v : mesh.vertices) {
        newVertices.push_back(v);
    }
    
    // For each selected face, duplicate vertices and move along normal
    for (int faceIdx : selection.selectedFaces) {
        int baseIdx = faceIdx * 3;
        if (baseIdx + 2 >= static_cast<int>(mesh.indices.size())) continue;
        
        unsigned int i0 = mesh.indices[baseIdx];
        unsigned int i1 = mesh.indices[baseIdx + 1];
        unsigned int i2 = mesh.indices[baseIdx + 2];
        
        glm::vec3 v0 = mesh.vertices[i0];
        glm::vec3 v1 = mesh.vertices[i1];
        glm::vec3 v2 = mesh.vertices[i2];
        
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        
        // Create new vertices
        int n0 = static_cast<int>(newVertices.size());
        newVertices.push_back(v0 + normal * distance);
        int n1 = static_cast<int>(newVertices.size());
        newVertices.push_back(v1 + normal * distance);
        int n2 = static_cast<int>(newVertices.size());
        newVertices.push_back(v2 + normal * distance);
        
        // Add new face
        newIndices.push_back(n0);
        newIndices.push_back(n1);
        newIndices.push_back(n2);
        
        // Add side faces
        newIndices.insert(newIndices.end(), {i0, i1, n1, n1, n0, i0});
        newIndices.insert(newIndices.end(), {i1, i2, n2, n2, n1, i1});
        newIndices.insert(newIndices.end(), {i2, i0, n0, n0, n2, i2});
    }
    
    // Keep non-selected faces
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        int faceIdx = static_cast<int>(i / 3);
        if (selection.selectedFaces.find(faceIdx) == selection.selectedFaces.end()) {
            newIndices.push_back(mesh.indices[i]);
            newIndices.push_back(mesh.indices[i + 1]);
            newIndices.push_back(mesh.indices[i + 2]);
        }
    }
    
    mesh.vertices = newVertices;
    mesh.indices = newIndices;
    mesh.calculateNormals();
    mesh.buildAdjacency();
}

void bevelEdges(Mesh& mesh, const Selection& selection, float amount) {
    // Simplified bevel: just offset edges slightly
    // A full bevel would require edge splitting and face creation
    (void)mesh; (void)selection; (void)amount;
    // Stub for now
}

void subdivide(Mesh& mesh) {
    // Basic Catmull-Clark subdivision
    std::map<std::pair<int, int>, int> edgeMidpoints;
    std::vector<glm::vec3> newVertices = mesh.vertices;
    std::vector<unsigned int> newIndices;
    
    auto getEdgeMidpoint = [&](int v0, int v1) {
        auto key = std::make_pair(std::min(v0, v1), std::max(v0, v1));
        auto it = edgeMidpoints.find(key);
        if (it != edgeMidpoints.end()) {
            return it->second;
        }
        glm::vec3 mid = (mesh.vertices[v0] + mesh.vertices[v1]) * 0.5f;
        int idx = static_cast<int>(newVertices.size());
        newVertices.push_back(mid);
        edgeMidpoints[key] = idx;
        return idx;
    };
    
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        int v0 = mesh.indices[i];
        int v1 = mesh.indices[i + 1];
        int v2 = mesh.indices[i + 2];
        
        int m01 = getEdgeMidpoint(v0, v1);
        int m12 = getEdgeMidpoint(v1, v2);
        int m20 = getEdgeMidpoint(v2, v0);
        
        newIndices.insert(newIndices.end(), {v0, m01, m20});
        newIndices.insert(newIndices.end(), {v1, m12, m01});
        newIndices.insert(newIndices.end(), {v2, m20, m12});
        newIndices.insert(newIndices.end(), {m01, m12, m20});
    }
    
    mesh.vertices = newVertices;
    mesh.indices = newIndices;
    mesh.calculateNormals();
    mesh.buildAdjacency();
}

void mergeByDistance(Mesh& mesh, float threshold) {
    std::map<int, int> mergeMap;
    float thresholdSq = threshold * threshold;
    
    for (size_t i = 0; i < mesh.vertices.size(); ++i) {
        if (mergeMap.find(static_cast<int>(i)) != mergeMap.end()) continue;
        
        for (size_t j = i + 1; j < mesh.vertices.size(); ++j) {
            if (mergeMap.find(static_cast<int>(j)) != mergeMap.end()) continue;
            
            float distSq = glm::distance2(mesh.vertices[i], mesh.vertices[j]);
            if (distSq < thresholdSq) {
                mergeMap[static_cast<int>(j)] = static_cast<int>(i);
            }
        }
    }
    
    for (auto& idx : mesh.indices) {
        auto it = mergeMap.find(idx);
        if (it != mergeMap.end()) {
            idx = it->second;
        }
    }
    
    mesh.calculateNormals();
    mesh.buildAdjacency();
}

void deleteVertices(Mesh& mesh, const Selection& selection) {
    // Remove faces that use selected vertices
    std::vector<unsigned int> newIndices;
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        unsigned int i0 = mesh.indices[i];
        unsigned int i1 = mesh.indices[i + 1];
        unsigned int i2 = mesh.indices[i + 2];
        
        if (selection.selectedVertices.find(i0) == selection.selectedVertices.end() &&
            selection.selectedVertices.find(i1) == selection.selectedVertices.end() &&
            selection.selectedVertices.find(i2) == selection.selectedVertices.end()) {
            newIndices.push_back(i0);
            newIndices.push_back(i1);
            newIndices.push_back(i2);
        }
    }
    mesh.indices = newIndices;
    mesh.calculateNormals();
    mesh.buildAdjacency();
}

void deleteEdges(Mesh& mesh, const Selection& selection) {
    (void)mesh; (void)selection;
    // Stub
}

void deleteFaces(Mesh& mesh, const Selection& selection) {
    std::vector<unsigned int> newIndices;
    for (size_t i = 0; i < mesh.indices.size(); i += 3) {
        int faceIdx = static_cast<int>(i / 3);
        if (selection.selectedFaces.find(faceIdx) == selection.selectedFaces.end()) {
            newIndices.push_back(mesh.indices[i]);
            newIndices.push_back(mesh.indices[i + 1]);
            newIndices.push_back(mesh.indices[i + 2]);
        }
    }
    mesh.indices = newIndices;
    mesh.calculateNormals();
    mesh.buildAdjacency();
}

void recalculateNormals(Mesh& mesh, bool smooth) {
    mesh.calculateNormals();
    (void)smooth; // Currently always smooth
}

} // namespace MeshOps

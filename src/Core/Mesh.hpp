#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Mesh {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    
    // GPU buffers
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int NBO = 0;
    unsigned int EBO = 0;
    
    // Adjacency for half-edge style operations
    std::vector<std::vector<int>> vertexToFaces;
    std::vector<std::vector<int>> faceToEdges;
    
    void uploadToGPU();
    void freeGPU();
    void calculateNormals();
    void buildAdjacency();
    
    static Mesh createCube(float size = 1.0f);
    static Mesh createPlane(float size = 10.0f, int subdivisions = 10);
    static Mesh createIcosphere(float radius = 1.0f, int subdivisions = 2);
};

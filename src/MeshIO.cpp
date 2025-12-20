#include "MeshIO.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace {
    struct VertexKey {
        int pos = -1;
        int uv = -1;
        int norm = -1;

        bool operator==(const VertexKey &other) const {
            return pos == other.pos && uv == other.uv && norm == other.norm;
        }
    };

    struct VertexKeyHash {
        std::size_t operator()(const VertexKey &key) const {
            return (static_cast<size_t>(key.pos) * 73856093) ^ (static_cast<size_t>(key.uv) * 19349663) ^ (static_cast<size_t>(key.norm) * 83492791);
        }
    };

}

namespace MeshIO {
    bool LoadOBJ(const std::wstring &path, Mesh &mesh, std::string &error) {
        std::ifstream file(path);
        if (!file) {
            error = "Unable to open OBJ file.";
            return false;
        }

        std::vector<DirectX::XMFLOAT3> positions;
        std::vector<DirectX::XMFLOAT3> normals;
        std::vector<DirectX::XMFLOAT2> uvs;

        std::unordered_map<VertexKey, uint32_t, VertexKeyHash> unique;
        mesh.vertices.clear();
        mesh.indices.clear();

        std::string line;
        while (std::getline(file, line)) {
            std::stringstream stream(line);
            std::string type;
            stream >> type;
            if (type == "v") {
                DirectX::XMFLOAT3 pos{};
                stream >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
            } else if (type == "vt") {
                DirectX::XMFLOAT2 uv{};
                stream >> uv.x >> uv.y;
                uvs.push_back(uv);
            } else if (type == "vn") {
                DirectX::XMFLOAT3 normal{};
                stream >> normal.x >> normal.y >> normal.z;
                normals.push_back(normal);
            } else if (type == "f") {
                std::string vert;
                std::vector<uint32_t> faceIndices;
                while (stream >> vert) {
                    VertexKey key{};
                    size_t first = vert.find('/');
                    size_t second = vert.find('/', first + 1);
                    if (first == std::string::npos) {
                        key.pos = std::stoi(vert) - 1;
                    } else {
                        key.pos = std::stoi(vert.substr(0, first)) - 1;
                        if (second == std::string::npos) {
                            key.uv = std::stoi(vert.substr(first + 1)) - 1;
                        } else {
                            if (second > first + 1) {
                                key.uv = std::stoi(vert.substr(first + 1, second - first - 1)) - 1;
                            }
                            if (second + 1 < vert.size()) {
                                key.norm = std::stoi(vert.substr(second + 1)) - 1;
                            }
                        }
                    }

                    auto it = unique.find(key);
                    if (it == unique.end()) {
                        Vertex vertex{};
                        if (key.pos >= 0 && key.pos < static_cast<int>(positions.size())) {
                            vertex.position = positions[key.pos];
                        }
                        if (key.uv >= 0 && key.uv < static_cast<int>(uvs.size())) {
                            vertex.uv = uvs[key.uv];
                        }
                        if (key.norm >= 0 && key.norm < static_cast<int>(normals.size())) {
                            vertex.normal = normals[key.norm];
                        }
                        uint32_t newIndex = static_cast<uint32_t>(mesh.vertices.size());
                        mesh.vertices.push_back(vertex);
                        unique[key] = newIndex;
                        faceIndices.push_back(newIndex);
                    } else {
                        faceIndices.push_back(it->second);
                    }
                }

                if (faceIndices.size() >= 3) {
                    for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                        mesh.indices.push_back(faceIndices[0]);
                        mesh.indices.push_back(faceIndices[i]);
                        mesh.indices.push_back(faceIndices[i + 1]);
                    }
                }
            }
        }

        if (mesh.vertices.empty() || mesh.indices.empty()) {
            error = "OBJ file contained no mesh data.";
            return false;
        }

        if (normals.empty()) {
            MeshOps::ComputeNormals(mesh);
        }

        return true;
    }

    bool SaveOBJ(const std::wstring &path, const Mesh &mesh, std::string &error) {
        std::ofstream file(path);
        if (!file) {
            error = "Unable to write OBJ file.";
            return false;
        }

        for (const auto &vertex : mesh.vertices) {
            file << "v " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << "\n";
        }
        for (const auto &vertex : mesh.vertices) {
            file << "vt " << vertex.uv.x << " " << vertex.uv.y << "\n";
        }
        for (const auto &vertex : mesh.vertices) {
            file << "vn " << vertex.normal.x << " " << vertex.normal.y << " " << vertex.normal.z << "\n";
        }

        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            uint32_t i0 = mesh.indices[i] + 1;
            uint32_t i1 = mesh.indices[i + 1] + 1;
            uint32_t i2 = mesh.indices[i + 2] + 1;
            file << "f "
                 << i0 << "/" << i0 << "/" << i0 << " "
                 << i1 << "/" << i1 << "/" << i1 << " "
                 << i2 << "/" << i2 << "/" << i2 << "\n";
        }

        return true;
    }
}

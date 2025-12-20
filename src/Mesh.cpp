#include "Mesh.h"

#include <DirectXMath.h>
#include <unordered_map>

using namespace DirectX;

namespace MeshOps {
    void ComputeNormals(Mesh &mesh) {
        for (auto &vertex : mesh.vertices) {
            vertex.normal = {0.0f, 0.0f, 0.0f};
        }

        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            uint32_t i0 = mesh.indices[i];
            uint32_t i1 = mesh.indices[i + 1];
            uint32_t i2 = mesh.indices[i + 2];

            XMVECTOR p0 = XMLoadFloat3(&mesh.vertices[i0].position);
            XMVECTOR p1 = XMLoadFloat3(&mesh.vertices[i1].position);
            XMVECTOR p2 = XMLoadFloat3(&mesh.vertices[i2].position);

            XMVECTOR n = XMVector3Normalize(XMVector3Cross(p1 - p0, p2 - p0));
            XMFLOAT3 normal;
            XMStoreFloat3(&normal, n);

            mesh.vertices[i0].normal.x += normal.x;
            mesh.vertices[i0].normal.y += normal.y;
            mesh.vertices[i0].normal.z += normal.z;
            mesh.vertices[i1].normal.x += normal.x;
            mesh.vertices[i1].normal.y += normal.y;
            mesh.vertices[i1].normal.z += normal.z;
            mesh.vertices[i2].normal.x += normal.x;
            mesh.vertices[i2].normal.y += normal.y;
            mesh.vertices[i2].normal.z += normal.z;
        }

        for (auto &vertex : mesh.vertices) {
            XMVECTOR n = XMLoadFloat3(&vertex.normal);
            n = XMVector3Normalize(n);
            XMStoreFloat3(&vertex.normal, n);
        }
    }

    void Subdivide(Mesh &mesh) {
        if (mesh.indices.size() < 3) {
            return;
        }

        std::vector<Vertex> newVertices = mesh.vertices;
        std::vector<uint32_t> newIndices;

        auto midpoint = [&](uint32_t a, uint32_t b) {
            const Vertex &va = mesh.vertices[a];
            const Vertex &vb = mesh.vertices[b];
            Vertex v;
            v.position = { (va.position.x + vb.position.x) * 0.5f,
                           (va.position.y + vb.position.y) * 0.5f,
                           (va.position.z + vb.position.z) * 0.5f };
            v.uv = { (va.uv.x + vb.uv.x) * 0.5f, (va.uv.y + vb.uv.y) * 0.5f };
            v.normal = {0.0f, 0.0f, 0.0f};
            newVertices.push_back(v);
            return static_cast<uint32_t>(newVertices.size() - 1);
        };

        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            uint32_t i0 = mesh.indices[i];
            uint32_t i1 = mesh.indices[i + 1];
            uint32_t i2 = mesh.indices[i + 2];

            uint32_t m0 = midpoint(i0, i1);
            uint32_t m1 = midpoint(i1, i2);
            uint32_t m2 = midpoint(i2, i0);

            newIndices.insert(newIndices.end(), {i0, m0, m2});
            newIndices.insert(newIndices.end(), {m0, i1, m1});
            newIndices.insert(newIndices.end(), {m2, m1, i2});
            newIndices.insert(newIndices.end(), {m0, m1, m2});
        }

        mesh.vertices.swap(newVertices);
        mesh.indices.swap(newIndices);
        ComputeNormals(mesh);
    }

    void ApplyTransform(Mesh &mesh, const Transform &transform) {
        XMMATRIX scale = XMMatrixScaling(transform.scale.x, transform.scale.y, transform.scale.z);
        XMMATRIX rot = XMMatrixRotationRollPitchYaw(transform.rotation.x, transform.rotation.y, transform.rotation.z);
        XMMATRIX trans = XMMatrixTranslation(transform.position.x, transform.position.y, transform.position.z);
        XMMATRIX m = scale * rot * trans;

        for (auto &vertex : mesh.vertices) {
            XMVECTOR p = XMLoadFloat3(&vertex.position);
            p = XMVector3Transform(p, m);
            XMStoreFloat3(&vertex.position, p);
        }
    }
}

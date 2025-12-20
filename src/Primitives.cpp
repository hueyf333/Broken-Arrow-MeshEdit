#include "Primitives.h"

#include <cmath>

namespace Primitives {
    Mesh MakeCube(float size) {
        Mesh mesh;
        float h = size * 0.5f;

        mesh.vertices = {
            {{-h, -h, -h}, {0, 0, -1}, {0, 1}},
            {{h, -h, -h}, {0, 0, -1}, {1, 1}},
            {{h, h, -h}, {0, 0, -1}, {1, 0}},
            {{-h, h, -h}, {0, 0, -1}, {0, 0}},

            {{-h, -h, h}, {0, 0, 1}, {0, 1}},
            {{h, -h, h}, {0, 0, 1}, {1, 1}},
            {{h, h, h}, {0, 0, 1}, {1, 0}},
            {{-h, h, h}, {0, 0, 1}, {0, 0}},

            {{-h, -h, -h}, {-1, 0, 0}, {0, 1}},
            {{-h, h, -h}, {-1, 0, 0}, {1, 1}},
            {{-h, h, h}, {-1, 0, 0}, {1, 0}},
            {{-h, -h, h}, {-1, 0, 0}, {0, 0}},

            {{h, -h, -h}, {1, 0, 0}, {0, 1}},
            {{h, h, -h}, {1, 0, 0}, {1, 1}},
            {{h, h, h}, {1, 0, 0}, {1, 0}},
            {{h, -h, h}, {1, 0, 0}, {0, 0}},

            {{-h, -h, -h}, {0, -1, 0}, {0, 1}},
            {{-h, -h, h}, {0, -1, 0}, {1, 1}},
            {{h, -h, h}, {0, -1, 0}, {1, 0}},
            {{h, -h, -h}, {0, -1, 0}, {0, 0}},

            {{-h, h, -h}, {0, 1, 0}, {0, 1}},
            {{-h, h, h}, {0, 1, 0}, {1, 1}},
            {{h, h, h}, {0, 1, 0}, {1, 0}},
            {{h, h, -h}, {0, 1, 0}, {0, 0}},
        };

        mesh.indices = {
            0, 1, 2, 0, 2, 3,
            4, 6, 5, 4, 7, 6,
            8, 9, 10, 8, 10, 11,
            12, 15, 14, 12, 14, 13,
            16, 17, 18, 16, 18, 19,
            20, 23, 22, 20, 22, 21,
        };

        return mesh;
    }

    Mesh MakePlane(float size) {
        Mesh mesh;
        float h = size * 0.5f;
        mesh.vertices = {
            {{-h, 0, -h}, {0, 1, 0}, {0, 1}},
            {{h, 0, -h}, {0, 1, 0}, {1, 1}},
            {{h, 0, h}, {0, 1, 0}, {1, 0}},
            {{-h, 0, h}, {0, 1, 0}, {0, 0}},
        };
        mesh.indices = {0, 1, 2, 0, 2, 3};
        return mesh;
    }

    Mesh MakeSphere(float radius, int slices, int stacks) {
        Mesh mesh;
        for (int stack = 0; stack <= stacks; ++stack) {
            float v = static_cast<float>(stack) / stacks;
            float phi = v * DirectX::XM_PI;
            for (int slice = 0; slice <= slices; ++slice) {
                float u = static_cast<float>(slice) / slices;
                float theta = u * DirectX::XM_2PI;

                float x = std::sin(phi) * std::cos(theta);
                float y = std::cos(phi);
                float z = std::sin(phi) * std::sin(theta);

                Vertex vert;
                vert.position = {x * radius, y * radius, z * radius};
                vert.normal = {x, y, z};
                vert.uv = {u, 1.0f - v};
                mesh.vertices.push_back(vert);
            }
        }

        int ringVertices = slices + 1;
        for (int stack = 0; stack < stacks; ++stack) {
            for (int slice = 0; slice < slices; ++slice) {
                int first = stack * ringVertices + slice;
                int second = first + ringVertices;
                mesh.indices.push_back(first);
                mesh.indices.push_back(second);
                mesh.indices.push_back(first + 1);

                mesh.indices.push_back(first + 1);
                mesh.indices.push_back(second);
                mesh.indices.push_back(second + 1);
            }
        }

        return mesh;
    }

    Mesh MakeCylinder(float radius, float height, int slices) {
        Mesh mesh;
        float half = height * 0.5f;
        for (int i = 0; i <= slices; ++i) {
            float t = static_cast<float>(i) / slices;
            float angle = t * DirectX::XM_2PI;
            float x = std::cos(angle) * radius;
            float z = std::sin(angle) * radius;
            mesh.vertices.push_back({{x, -half, z}, {x, 0.0f, z}, {t, 1.0f}});
            mesh.vertices.push_back({{x, half, z}, {x, 0.0f, z}, {t, 0.0f}});
        }

        for (int i = 0; i < slices; ++i) {
            uint32_t base = i * 2;
            mesh.indices.push_back(base);
            mesh.indices.push_back(base + 1);
            mesh.indices.push_back(base + 2);

            mesh.indices.push_back(base + 2);
            mesh.indices.push_back(base + 1);
            mesh.indices.push_back(base + 3);
        }

        MeshOps::ComputeNormals(mesh);
        return mesh;
    }
}

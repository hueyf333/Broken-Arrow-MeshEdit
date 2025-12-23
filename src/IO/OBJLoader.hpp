#pragma once
#include "Core/Mesh.hpp"
#include <string>

namespace OBJLoader {
    bool load(const std::string& path, Mesh& mesh);
}

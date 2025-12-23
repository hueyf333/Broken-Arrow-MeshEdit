#pragma once
#include "Core/Mesh.hpp"
#include <string>

namespace OBJWriter {
    bool save(const std::string& path, const Mesh& mesh);
}

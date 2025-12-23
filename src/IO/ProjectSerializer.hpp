#pragma once
#include "Core/Scene.hpp"
#include <string>

namespace ProjectSerializer {
    bool save(const std::string& path, const Scene& scene);
    bool load(const std::string& path, Scene& scene);
}

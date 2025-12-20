#pragma once

#include "Mesh.h"
#include <string>

namespace MeshIO {
    bool LoadOBJ(const std::wstring &path, Mesh &mesh, std::string &error);
    bool SaveOBJ(const std::wstring &path, const Mesh &mesh, std::string &error);
}

// File: MeshEditor/src/assets/Importer.h
#pragma once

#include "scene/Scene.h"
#include <string>

namespace MeshEditor
{
    class Importer
    {
    public:
        static bool ImportMesh(const std::string& path, Scene& scene, Entity*& outEntity, std::string& outError);
    };
}

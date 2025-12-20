// File: MeshEditor/src/assets/Importer.h
#pragma once

#include <string>

namespace MeshEditor
{
class Scene;
class UndoStack;

class Importer
{
public:
    bool Import(const std::string& filePath, Scene& scene, UndoStack& undo);
};
}

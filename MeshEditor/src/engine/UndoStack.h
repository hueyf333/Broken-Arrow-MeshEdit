// File: MeshEditor/src/engine/UndoStack.h
#pragma once

#include <vector>
#include "Command.h"

namespace mesh
{
class UndoStack
{
public:
    void Execute(CommandPtr command);
    void Undo();
    void Redo();

private:
    std::vector<CommandPtr> stack_;
    size_t index_ = 0;
};
}

// File: MeshEditor/src/engine/UndoStack.h
#pragma once

#include <vector>
#include <memory>
#include "Command.h"

namespace MeshEditor
{
class UndoStack
{
public:
    void Execute(std::unique_ptr<Command> command);
    void Undo();
    void Redo();

private:
    std::vector<std::unique_ptr<Command>> m_commands;
    int m_index = -1;
};
}

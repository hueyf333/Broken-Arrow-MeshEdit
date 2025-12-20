// File: MeshEditor/src/engine/Command.cpp
#include "Command.h"

namespace mesh
{
FunctionCommand::FunctionCommand(std::string name, std::function<void()> execute, std::function<void()> undo)
    : name_(std::move(name)), execute_(std::move(execute)), undo_(std::move(undo))
{
}

void FunctionCommand::Execute()
{
    if (execute_)
    {
        execute_();
    }
}

void FunctionCommand::Undo()
{
    if (undo_)
    {
        undo_();
    }
}

std::string FunctionCommand::Name() const
{
    return name_;
}
}

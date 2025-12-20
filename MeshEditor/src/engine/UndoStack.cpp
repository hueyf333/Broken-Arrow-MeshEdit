// File: MeshEditor/src/engine/UndoStack.cpp
#include "UndoStack.h"

namespace mesh
{
void UndoStack::Execute(CommandPtr command)
{
    if (!command)
    {
        return;
    }
    if (index_ < stack_.size())
    {
        stack_.erase(stack_.begin() + static_cast<long long>(index_), stack_.end());
    }
    command->Execute();
    stack_.push_back(std::move(command));
    index_ = stack_.size();
}

void UndoStack::Undo()
{
    if (index_ == 0)
    {
        return;
    }
    --index_;
    stack_[index_]->Undo();
}

void UndoStack::Redo()
{
    if (index_ >= stack_.size())
    {
        return;
    }
    stack_[index_]->Execute();
    ++index_;
}
}

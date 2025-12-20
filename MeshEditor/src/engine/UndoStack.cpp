// File: MeshEditor/src/engine/UndoStack.cpp
#include "engine/UndoStack.h"

namespace MeshEditor
{
    void UndoStack::Push(Command command)
    {
        if (mUndo.size() >= mMaxSize)
        {
            mUndo.pop_front();
        }
        mUndo.emplace_back(std::move(command));
        mRedo.clear();
        mUndo.back().Execute();
    }

    bool UndoStack::CanUndo() const
    {
        return !mUndo.empty();
    }

    bool UndoStack::CanRedo() const
    {
        return !mRedo.empty();
    }

    void UndoStack::Undo()
    {
        if (mUndo.empty())
        {
            return;
        }
        Command command = std::move(mUndo.back());
        mUndo.pop_back();
        command.Undo();
        mRedo.emplace_back(std::move(command));
    }

    void UndoStack::Redo()
    {
        if (mRedo.empty())
        {
            return;
        }
        Command command = std::move(mRedo.back());
        mRedo.pop_back();
        command.Execute();
        mUndo.emplace_back(std::move(command));
    }
}

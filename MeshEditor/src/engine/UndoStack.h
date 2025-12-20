// File: MeshEditor/src/engine/UndoStack.h
#pragma once

#include "engine/Command.h"
#include <deque>

namespace MeshEditor
{
    class UndoStack
    {
    public:
        void Push(Command command);
        bool CanUndo() const;
        bool CanRedo() const;
        void Undo();
        void Redo();

    private:
        std::deque<Command> mUndo;
        std::deque<Command> mRedo;
        size_t mMaxSize = 128;
    };
}

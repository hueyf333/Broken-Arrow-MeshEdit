// File: MeshEditor/src/engine/Command.cpp
#include "engine/Command.h"

namespace MeshEditor
{
    Command::Command(std::string name, std::function<void()> execute, std::function<void()> undo)
        : mName(std::move(name)), mExecute(std::move(execute)), mUndo(std::move(undo))
    {
    }

    void Command::Execute()
    {
        if (mExecute)
        {
            mExecute();
        }
    }

    void Command::Undo()
    {
        if (mUndo)
        {
            mUndo();
        }
    }

    const std::string& Command::Name() const
    {
        return mName;
    }
}

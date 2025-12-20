// File: MeshEditor/src/engine/Command.h
#pragma once

#include <functional>
#include <string>

namespace MeshEditor
{
    class Command
    {
    public:
        Command() = default;
        Command(std::string name, std::function<void()> execute, std::function<void()> undo);

        void Execute();
        void Undo();
        const std::string& Name() const;

    private:
        std::string mName;
        std::function<void()> mExecute;
        std::function<void()> mUndo;
    };
}

// File: MeshEditor/src/engine/Logger.h
#pragma once

#include <functional>
#include <string>

namespace MeshEditor
{
    class Logger
    {
    public:
        using Callback = std::function<void(const std::string&)>;

        static void SetCallback(Callback callback);
        static void Info(const std::string& message);
        static void Warn(const std::string& message);
        static void Error(const std::string& message);

    private:
        static void Emit(const std::string& message);
    };
}

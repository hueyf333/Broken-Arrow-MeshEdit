// File: MeshEditor/src/engine/Logger.cpp
#include "engine/Logger.h"

#include <iostream>
#include <mutex>

namespace MeshEditor
{
    namespace
    {
        Logger::Callback gCallback;
        std::mutex gMutex;
    }

    void Logger::SetCallback(Callback callback)
    {
        std::scoped_lock lock(gMutex);
        gCallback = std::move(callback);
    }

    void Logger::Info(const std::string& message)
    {
        Emit("[Info] " + message);
    }

    void Logger::Warn(const std::string& message)
    {
        Emit("[Warn] " + message);
    }

    void Logger::Error(const std::string& message)
    {
        Emit("[Error] " + message);
    }

    void Logger::Emit(const std::string& message)
    {
        std::scoped_lock lock(gMutex);
        if (gCallback)
        {
            gCallback(message);
        }
        else
        {
            std::cout << message << std::endl;
        }
    }
}

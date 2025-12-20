// File: MeshEditor/src/engine/Logger.cpp
#include "Logger.h"
#include <iostream>

namespace MeshEditor
{
Logger::Callback Logger::s_callback = nullptr;

void Logger::Info(const std::string& message)
{
    if (s_callback)
    {
        s_callback(message);
    }
    else
    {
        std::cout << message << std::endl;
    }
}

void Logger::SetCallback(Callback callback)
{
    s_callback = std::move(callback);
}
}

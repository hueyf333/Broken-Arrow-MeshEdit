// File: MeshEditor/src/engine/Logger.h
#pragma once

#include <string>
#include <functional>

namespace MeshEditor
{
class Logger
{
public:
    using Callback = std::function<void(const std::string&)>;

    static void Info(const std::string& message);
    static void SetCallback(Callback callback);

private:
    static Callback s_callback;
};
}

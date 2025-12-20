// File: MeshEditor/src/tools/ToolManager.h
#pragma once

#include "tools/Tool.h"
#include <memory>
#include <unordered_map>

namespace MeshEditor
{
    class ToolManager
    {
    public:
        void Register(std::unique_ptr<Tool> tool);
        Tool* ActiveTool() const;
        void Activate(const QString& name, ViewportWidget* viewport);

    private:
        std::unordered_map<QString, std::unique_ptr<Tool>> mTools;
        Tool* mActive = nullptr;
    };
}

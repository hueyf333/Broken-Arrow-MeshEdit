// File: MeshEditor/src/tools/ToolManager.cpp
#include "tools/ToolManager.h"
#include "ui/ViewportWidget.h"

namespace MeshEditor
{
    void ToolManager::Register(std::unique_ptr<Tool> tool)
    {
        mTools.emplace(tool->Name(), std::move(tool));
    }

    Tool* ToolManager::ActiveTool() const
    {
        return mActive;
    }

    void ToolManager::Activate(const QString& name, ViewportWidget* viewport)
    {
        if (mActive)
        {
            mActive->OnDeactivate(viewport);
        }
        auto it = mTools.find(name);
        if (it != mTools.end())
        {
            mActive = it->second.get();
            mActive->OnActivate(viewport);
        }
    }
}

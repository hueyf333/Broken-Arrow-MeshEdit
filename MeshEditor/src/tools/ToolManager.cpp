// File: MeshEditor/src/tools/ToolManager.cpp
#include "ToolManager.h"

namespace mesh
{
void ToolManager::Register(std::unique_ptr<Tool> tool)
{
    if (!tool)
    {
        return;
    }
    tools_[tool->Type()] = std::move(tool);
}

void ToolManager::SetActive(ToolType type)
{
    auto it = tools_.find(type);
    if (it == tools_.end())
    {
        return;
    }
    if (active_)
    {
        active_->OnDeactivate();
    }
    active_ = it->second.get();
    if (active_)
    {
        active_->OnActivate();
    }
}

Tool* ToolManager::ActiveTool() const
{
    return active_;
}

QString ToolManager::ActiveToolName() const
{
    return active_ ? active_->Name() : QStringLiteral("None");
}

void ToolManager::OnMousePress(QMouseEvent* event)
{
    if (active_)
    {
        active_->OnMousePress(event);
    }
}

void ToolManager::OnMouseMove(QMouseEvent* event)
{
    if (active_)
    {
        active_->OnMouseMove(event);
    }
}

void ToolManager::OnMouseRelease(QMouseEvent* event)
{
    if (active_)
    {
        active_->OnMouseRelease(event);
    }
}
}

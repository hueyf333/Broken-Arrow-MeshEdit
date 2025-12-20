// File: MeshEditor/src/tools/ToolManager.cpp
#include "ToolManager.h"
#include "Tool_Select.h"
#include "Tool_Transform.h"
#include "Tool_Extrude.h"
#include "Tool_Bevel.h"

namespace MeshEditor
{
ToolManager::ToolManager()
    : m_active(nullptr)
    , m_undo(nullptr)
{
    m_tools["Select"] = std::make_unique<Tool_Select>();
    m_tools["Move"] = std::make_unique<Tool_Transform>(Tool_Transform::Mode::Move);
    m_tools["Rotate"] = std::make_unique<Tool_Transform>(Tool_Transform::Mode::Rotate);
    m_tools["Scale"] = std::make_unique<Tool_Transform>(Tool_Transform::Mode::Scale);
    m_tools["Extrude"] = std::make_unique<Tool_Extrude>();
    m_tools["Bevel"] = std::make_unique<Tool_Bevel>();

    SetActiveTool("Select");
}

void ToolManager::SetActiveTool(const std::string& name)
{
    if (m_active)
        m_active->OnDeactivate();

    auto it = m_tools.find(name);
    if (it != m_tools.end())
    {
        m_active = it->second.get();
        m_active->OnActivate();
    }
}

std::string ToolManager::GetActiveToolName() const
{
    if (!m_active)
        return "None";
    return m_active->GetName();
}

void ToolManager::OnMousePress(QMouseEvent* event, Scene* scene)
{
    if (m_active)
        m_active->OnMousePress(event, scene);
}

void ToolManager::OnMouseMove(QMouseEvent* event, Scene* scene)
{
    if (m_active)
        m_active->OnMouseMove(event, scene);
}

void ToolManager::OnMouseRelease(QMouseEvent* event, Scene* scene)
{
    if (m_active)
        m_active->OnMouseRelease(event, scene);
}

void ToolManager::BindUndoStack(UndoStack* undo)
{
    m_undo = undo;
}
}

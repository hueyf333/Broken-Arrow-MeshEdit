// File: MeshEditor/src/tools/ToolManager.h
#pragma once

#include <unordered_map>
#include <memory>
#include "Tool.h"

namespace MeshEditor
{
class UndoStack;

class ToolManager
{
public:
    ToolManager();
    void SetActiveTool(const std::string& name);
    std::string GetActiveToolName() const;

    void OnMousePress(QMouseEvent* event, Scene* scene);
    void OnMouseMove(QMouseEvent* event, Scene* scene);
    void OnMouseRelease(QMouseEvent* event, Scene* scene);

    void BindUndoStack(UndoStack* undo);

private:
    std::unordered_map<std::string, std::unique_ptr<Tool>> m_tools;
    Tool* m_active;
    UndoStack* m_undo;
};
}

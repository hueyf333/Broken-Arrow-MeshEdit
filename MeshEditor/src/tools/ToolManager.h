// File: MeshEditor/src/tools/ToolManager.h
#pragma once

#include <memory>
#include <unordered_map>

#include "Tool.h"

namespace mesh
{
class ToolManager
{
public:
    void Register(std::unique_ptr<Tool> tool);
    void SetActive(ToolType type);
    Tool* ActiveTool() const;
    QString ActiveToolName() const;

    void OnMousePress(QMouseEvent* event);
    void OnMouseMove(QMouseEvent* event);
    void OnMouseRelease(QMouseEvent* event);

private:
    std::unordered_map<ToolType, std::unique_ptr<Tool>> tools_;
    Tool* active_ = nullptr;
};
}

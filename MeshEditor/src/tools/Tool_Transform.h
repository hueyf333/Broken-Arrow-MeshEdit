// File: MeshEditor/src/tools/Tool_Transform.h
#pragma once

#include "Tool.h"
#include "scene/Scene.h"
#include "scene/Selection.h"
#include "ui/ViewportWidget.h"
#include <glm/glm.hpp>
#include "engine/UndoStack.h"

namespace mesh
{
class Tool_Transform : public Tool
{
public:
    Tool_Transform(Scene& scene, Selection& selection, ViewportWidget& viewport, UndoStack& undoStack, ToolType type);

    ToolType Type() const override { return type_; }
    QString Name() const override;

    void OnMousePress(QMouseEvent* event) override;
    void OnMouseMove(QMouseEvent* event) override;
    void OnMouseRelease(QMouseEvent* event) override;

private:
    Scene& scene_;
    Selection& selection_;
    ViewportWidget& viewport_;
    UndoStack& undoStack_;
    ToolType type_;
    bool dragging_ = false;
    QPoint dragStart_;
    glm::vec3 startPosition_{};
};
}

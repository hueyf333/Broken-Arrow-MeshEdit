// File: MeshEditor/src/tools/Tool_Transform.h
#pragma once

#include "Tool.h"
#include "engine/Core.h"

namespace MeshEditor
{
class Tool_Transform : public Tool
{
public:
    enum class Mode { Move, Rotate, Scale };

    explicit Tool_Transform(Mode mode);
    std::string GetName() const override;
    void OnMousePress(QMouseEvent* event, Scene* scene) override;
    void OnMouseMove(QMouseEvent* event, Scene* scene) override;
    void OnMouseRelease(QMouseEvent* event, Scene* scene) override;

private:
    Mode m_mode;
    bool m_dragging;
    QPoint m_lastPos;
};
}

// File: MeshEditor/src/tools/Tool.h
#pragma once

#include <QString>
#include <QMouseEvent>

namespace mesh
{
enum class ToolType
{
    Select,
    Move,
    Rotate,
    Scale,
    Extrude,
    Bevel
};

class Tool
{
public:
    virtual ~Tool() = default;
    virtual ToolType Type() const = 0;
    virtual QString Name() const = 0;

    virtual void OnActivate() {}
    virtual void OnDeactivate() {}
    virtual void OnMousePress(QMouseEvent*) {}
    virtual void OnMouseMove(QMouseEvent*) {}
    virtual void OnMouseRelease(QMouseEvent*) {}
    virtual void DrawOverlay() {}
};
}

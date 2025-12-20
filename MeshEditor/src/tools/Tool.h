// File: MeshEditor/src/tools/Tool.h
#pragma once

#include <string>
#include <QMouseEvent>

namespace MeshEditor
{
class Scene;

class Tool
{
public:
    virtual ~Tool() = default;
    virtual std::string GetName() const = 0;
    virtual void OnActivate() {}
    virtual void OnDeactivate() {}
    virtual void OnMousePress(QMouseEvent* event, Scene* scene) {}
    virtual void OnMouseMove(QMouseEvent* event, Scene* scene) {}
    virtual void OnMouseRelease(QMouseEvent* event, Scene* scene) {}
    virtual void DrawOverlay() {}
};
}

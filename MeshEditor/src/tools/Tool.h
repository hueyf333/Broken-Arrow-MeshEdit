// File: MeshEditor/src/tools/Tool.h
#pragma once

#include <QString>
#include <QMouseEvent>
#include <QPainter>

namespace MeshEditor
{
    class ViewportWidget;

    class Tool
    {
    public:
        virtual ~Tool() = default;
        virtual QString Name() const = 0;
        virtual void OnActivate(ViewportWidget*) {}
        virtual void OnDeactivate(ViewportWidget*) {}
        virtual void OnMousePress(ViewportWidget*, QMouseEvent*) {}
        virtual void OnMouseMove(ViewportWidget*, QMouseEvent*) {}
        virtual void OnMouseRelease(ViewportWidget*, QMouseEvent*) {}
        virtual void DrawOverlay(ViewportWidget*, QPainter&) {}
    };
}

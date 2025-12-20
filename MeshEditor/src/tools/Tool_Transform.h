// File: MeshEditor/src/tools/Tool_Transform.h
#pragma once

#include "tools/Tool.h"

namespace MeshEditor
{
    enum class TransformMode
    {
        Move,
        Rotate,
        Scale
    };

    class Tool_Transform : public Tool
    {
    public:
        explicit Tool_Transform(TransformMode mode);
        QString Name() const override;
        void OnMousePress(ViewportWidget* viewport, QMouseEvent* event) override;
        void OnMouseMove(ViewportWidget* viewport, QMouseEvent* event) override;
        void OnMouseRelease(ViewportWidget* viewport, QMouseEvent* event) override;
        void DrawOverlay(ViewportWidget* viewport, QPainter& painter) override;

    private:
        TransformMode mMode;
        bool mDragging = false;
        QPoint mStart;
    };
}

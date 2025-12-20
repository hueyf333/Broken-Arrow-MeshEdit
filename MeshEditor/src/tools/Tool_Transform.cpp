// File: MeshEditor/src/tools/Tool_Transform.cpp
#include "tools/Tool_Transform.h"
#include "ui/ViewportWidget.h"

namespace MeshEditor
{
    Tool_Transform::Tool_Transform(TransformMode mode)
        : mMode(mode)
    {
    }

    QString Tool_Transform::Name() const
    {
        switch (mMode)
        {
        case TransformMode::Move:
            return "Move";
        case TransformMode::Rotate:
            return "Rotate";
        case TransformMode::Scale:
            return "Scale";
        default:
            return "Transform";
        }
    }

    void Tool_Transform::OnMousePress(ViewportWidget* viewport, QMouseEvent* event)
    {
        if (event->button() == Qt::LeftButton)
        {
            mDragging = true;
            mStart = event->pos();
            viewport->BeginTransform(mMode);
        }
    }

    void Tool_Transform::OnMouseMove(ViewportWidget* viewport, QMouseEvent* event)
    {
        if (!mDragging)
        {
            return;
        }
        QPoint delta = event->pos() - mStart;
        viewport->UpdateTransform(mMode, delta);
    }

    void Tool_Transform::OnMouseRelease(ViewportWidget* viewport, QMouseEvent* event)
    {
        if (mDragging && event->button() == Qt::LeftButton)
        {
            mDragging = false;
            viewport->EndTransform(mMode);
        }
    }

    void Tool_Transform::DrawOverlay(ViewportWidget* viewport, QPainter& painter)
    {
        viewport->DrawGizmoOverlay(mMode, painter);
    }
}

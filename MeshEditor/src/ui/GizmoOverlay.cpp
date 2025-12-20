// File: MeshEditor/src/ui/GizmoOverlay.cpp
#include "ui/GizmoOverlay.h"

namespace MeshEditor
{
    void GizmoOverlay::DrawAxis(QPainter& painter, const QPoint& center, int size)
    {
        QPen penX(Qt::red, 2);
        QPen penY(Qt::green, 2);
        QPen penZ(Qt::blue, 2);

        painter.setPen(penX);
        painter.drawLine(center, center + QPoint(size, 0));
        painter.setPen(penY);
        painter.drawLine(center, center + QPoint(0, -size));
        painter.setPen(penZ);
        painter.drawLine(center, center + QPoint(-size / 2, size / 2));
    }
}

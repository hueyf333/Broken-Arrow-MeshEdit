// File: MeshEditor/src/ui/GizmoOverlay.h
#pragma once

#include <QPainter>
#include <glm/glm.hpp>

namespace MeshEditor
{
    class GizmoOverlay
    {
    public:
        void DrawAxis(QPainter& painter, const QPoint& center, int size);
    };
}

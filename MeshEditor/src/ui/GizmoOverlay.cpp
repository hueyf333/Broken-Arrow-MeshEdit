// File: MeshEditor/src/ui/GizmoOverlay.cpp
#include "GizmoOverlay.h"
#include <QPainter>

namespace MeshEditor
{
GizmoOverlay::GizmoOverlay(QWidget* parent)
    : QWidget(parent)
    , m_axis(1.0f, 0.0f, 0.0f)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void GizmoOverlay::SetAxis(const glm::vec3& axis)
{
    m_axis = axis;
    update();
}

void GizmoOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::red, 2));
    QPoint center(width() / 2, height() / 2);
    painter.drawLine(center, QPoint(center.x() + 40, center.y()));
}
}

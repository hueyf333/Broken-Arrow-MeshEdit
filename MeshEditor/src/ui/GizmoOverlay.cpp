// File: MeshEditor/src/ui/GizmoOverlay.cpp
#include "GizmoOverlay.h"

#include <QPainter>

namespace mesh
{
GizmoOverlay::GizmoOverlay(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setAttribute(Qt::WA_NoSystemBackground);
}

void GizmoOverlay::SetOrigin(const QPoint& screenPos)
{
    origin_ = screenPos;
    update();
}

void GizmoOverlay::SetVisibleAxis(bool x, bool y, bool z)
{
    showX_ = x;
    showY_ = y;
    showZ_ = z;
    update();
}

void GizmoOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int length = 60;

    if (showX_)
    {
        painter.setPen(QPen(Qt::red, 2));
        painter.drawLine(origin_, origin_ + QPoint(length, 0));
    }
    if (showY_)
    {
        painter.setPen(QPen(Qt::green, 2));
        painter.drawLine(origin_, origin_ + QPoint(0, -length));
    }
    if (showZ_)
    {
        painter.setPen(QPen(Qt::blue, 2));
        painter.drawLine(origin_, origin_ + QPoint(-length / 2, length / 2));
    }
}
}

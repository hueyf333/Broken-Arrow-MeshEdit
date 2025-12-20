// File: MeshEditor/src/ui/GizmoOverlay.h
#pragma once

#include <QWidget>
#include <glm/glm.hpp>

namespace mesh
{
class GizmoOverlay : public QWidget
{
public:
    explicit GizmoOverlay(QWidget* parent = nullptr);

    void SetOrigin(const QPoint& screenPos);
    void SetVisibleAxis(bool x, bool y, bool z);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QPoint origin_;
    bool showX_ = true;
    bool showY_ = true;
    bool showZ_ = true;
};
}

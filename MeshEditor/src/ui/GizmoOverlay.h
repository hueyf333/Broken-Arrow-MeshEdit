// File: MeshEditor/src/ui/GizmoOverlay.h
#pragma once

#include <QWidget>
#include <glm/vec3.hpp>

namespace MeshEditor
{
class GizmoOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit GizmoOverlay(QWidget* parent = nullptr);

    void SetAxis(const glm::vec3& axis);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    glm::vec3 m_axis;
};
}

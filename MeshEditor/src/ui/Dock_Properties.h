// File: MeshEditor/src/ui/Dock_Properties.h
#pragma once

#include <QDockWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColor>
#include <functional>

#include "scene/Scene.h"
#include "scene/Selection.h"

namespace mesh
{
class Dock_Properties : public QDockWidget
{
public:
    explicit Dock_Properties(QWidget* parent = nullptr);

    void SetSelection(const Scene& scene, const Selection& selection);
    void ApplyToEntity(Entity& entity);
    void SetOnEdited(const std::function<void()>& callback);

private:
    void OnColorPick();
    void OnTextureBrowse();
    void SetSpinValues(const TransformComponent& transform);

    QDoubleSpinBox* posX_ = nullptr;
    QDoubleSpinBox* posY_ = nullptr;
    QDoubleSpinBox* posZ_ = nullptr;
    QDoubleSpinBox* rotX_ = nullptr;
    QDoubleSpinBox* rotY_ = nullptr;
    QDoubleSpinBox* rotZ_ = nullptr;
    QDoubleSpinBox* scaleX_ = nullptr;
    QDoubleSpinBox* scaleY_ = nullptr;
    QDoubleSpinBox* scaleZ_ = nullptr;

    QLineEdit* texturePath_ = nullptr;
    QPushButton* colorButton_ = nullptr;
    QColor baseColor_;
    std::function<void()> onEdited_;
};
}

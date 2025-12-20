// File: MeshEditor/src/ui/Dock_Properties.h
#pragma once

#include <QDockWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFormLayout>
#include <QColorDialog>

namespace MeshEditor
{
class Scene;
class UndoStack;

class Dock_Properties : public QDockWidget
{
    Q_OBJECT

public:
    explicit Dock_Properties(QWidget* parent = nullptr);
    void BindScene(Scene* scene);
    void BindUndoStack(UndoStack* undo);
    void Refresh();

public slots:
    void OnSelectionChanged();

private slots:
    void OnTransformEdited();
    void OnColorPick();
    void OnLoadTexture();

private:
    Scene* m_scene;
    UndoStack* m_undo;

    QDoubleSpinBox* m_pos[3];
    QDoubleSpinBox* m_rot[3];
    QDoubleSpinBox* m_scale[3];
    QPushButton* m_colorButton;
    QLineEdit* m_texturePath;
    QPushButton* m_textureButton;
};
}

// File: MeshEditor/src/ui/Dock_Properties.cpp
#include "Dock_Properties.h"
#include "scene/Scene.h"
#include "scene/Entity.h"
#include "engine/UndoStack.h"
#include "engine/Command.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <glm/glm.hpp>

namespace MeshEditor
{
Dock_Properties::Dock_Properties(QWidget* parent)
    : QDockWidget("Properties", parent)
    , m_scene(nullptr)
    , m_undo(nullptr)
{
    QWidget* container = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(container);

    QFormLayout* transformLayout = new QFormLayout();

    for (int i = 0; i < 3; ++i)
    {
        m_pos[i] = new QDoubleSpinBox();
        m_pos[i]->setRange(-10000.0, 10000.0);
        m_rot[i] = new QDoubleSpinBox();
        m_rot[i]->setRange(-360.0, 360.0);
        m_scale[i] = new QDoubleSpinBox();
        m_scale[i]->setRange(0.001, 1000.0);
        m_scale[i]->setValue(1.0);
    }

    QWidget* posRow = new QWidget();
    QHBoxLayout* posLayout = new QHBoxLayout(posRow);
    for (int i = 0; i < 3; ++i) posLayout->addWidget(m_pos[i]);
    transformLayout->addRow("Position", posRow);

    QWidget* rotRow = new QWidget();
    QHBoxLayout* rotLayout = new QHBoxLayout(rotRow);
    for (int i = 0; i < 3; ++i) rotLayout->addWidget(m_rot[i]);
    transformLayout->addRow("Rotation", rotRow);

    QWidget* scaleRow = new QWidget();
    QHBoxLayout* scaleLayout = new QHBoxLayout(scaleRow);
    for (int i = 0; i < 3; ++i) scaleLayout->addWidget(m_scale[i]);
    transformLayout->addRow("Scale", scaleRow);

    layout->addLayout(transformLayout);

    m_colorButton = new QPushButton("Base Color");
    m_texturePath = new QLineEdit();
    m_textureButton = new QPushButton("Load Texture");

    layout->addWidget(m_colorButton);
    layout->addWidget(m_texturePath);
    layout->addWidget(m_textureButton);

    setWidget(container);

    for (int i = 0; i < 3; ++i)
    {
        connect(m_pos[i], &QDoubleSpinBox::editingFinished, this, &Dock_Properties::OnTransformEdited);
        connect(m_rot[i], &QDoubleSpinBox::editingFinished, this, &Dock_Properties::OnTransformEdited);
        connect(m_scale[i], &QDoubleSpinBox::editingFinished, this, &Dock_Properties::OnTransformEdited);
    }
    connect(m_colorButton, &QPushButton::clicked, this, &Dock_Properties::OnColorPick);
    connect(m_textureButton, &QPushButton::clicked, this, &Dock_Properties::OnLoadTexture);
}

void Dock_Properties::BindScene(Scene* scene)
{
    m_scene = scene;
    Refresh();
}

void Dock_Properties::BindUndoStack(UndoStack* undo)
{
    m_undo = undo;
}

void Dock_Properties::Refresh()
{
    if (!m_scene)
        return;

    auto* entity = m_scene->GetSelection().GetPrimary();
    if (!entity)
        return;

    const auto& t = entity->GetTransform();
    m_pos[0]->setValue(t.position.x);
    m_pos[1]->setValue(t.position.y);
    m_pos[2]->setValue(t.position.z);
    m_rot[0]->setValue(t.rotation.x);
    m_rot[1]->setValue(t.rotation.y);
    m_rot[2]->setValue(t.rotation.z);
    m_scale[0]->setValue(t.scale.x);
    m_scale[1]->setValue(t.scale.y);
    m_scale[2]->setValue(t.scale.z);

    m_texturePath->setText(QString::fromStdString(entity->GetMaterial().baseColorTexture));
}

void Dock_Properties::OnSelectionChanged()
{
    Refresh();
}

void Dock_Properties::OnTransformEdited()
{
    if (!m_scene || !m_undo)
        return;

    auto* entity = m_scene->GetSelection().GetPrimary();
    if (!entity)
        return;

    Transform before = entity->GetTransform();
    Transform after = before;
    after.position = glm::vec3(m_pos[0]->value(), m_pos[1]->value(), m_pos[2]->value());
    after.rotation = glm::vec3(m_rot[0]->value(), m_rot[1]->value(), m_rot[2]->value());
    after.scale = glm::vec3(m_scale[0]->value(), m_scale[1]->value(), m_scale[2]->value());

    auto cmd = std::make_unique<TransformCommand>(entity->GetId(), before, after, m_scene);
    m_undo->Execute(std::move(cmd));
}

void Dock_Properties::OnColorPick()
{
    if (!m_scene)
        return;

    auto* entity = m_scene->GetSelection().GetPrimary();
    if (!entity)
        return;

    QColor color = QColorDialog::getColor(Qt::white, this, "Base Color");
    if (!color.isValid())
        return;

    entity->GetMaterial().baseColor = glm::vec4(color.redF(), color.greenF(), color.blueF(), 1.0f);
}

void Dock_Properties::OnLoadTexture()
{
    if (!m_scene)
        return;

    auto* entity = m_scene->GetSelection().GetPrimary();
    if (!entity)
        return;

    QString filePath = QFileDialog::getOpenFileName(this, "Load Texture", QString(), "Textures (*.png *.jpg *.jpeg)");
    if (filePath.isEmpty())
        return;

    entity->GetMaterial().baseColorTexture = filePath.toStdString();
    m_texturePath->setText(filePath);
}
}

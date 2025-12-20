// File: MeshEditor/src/ui/Dock_Properties.cpp
#include "Dock_Properties.h"

#include <QColorDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <glm/glm.hpp>

namespace mesh
{
static QDoubleSpinBox* CreateSpin()
{
    auto* spin = new QDoubleSpinBox();
    spin->setRange(-9999.0, 9999.0);
    spin->setDecimals(3);
    spin->setSingleStep(0.1);
    return spin;
}

Dock_Properties::Dock_Properties(QWidget* parent)
    : QDockWidget("Properties", parent)
{
    auto* root = new QWidget(this);
    auto* layout = new QVBoxLayout(root);

    auto* transformGroup = new QGroupBox("Transform", root);
    auto* form = new QFormLayout(transformGroup);

    posX_ = CreateSpin();
    posY_ = CreateSpin();
    posZ_ = CreateSpin();
    rotX_ = CreateSpin();
    rotY_ = CreateSpin();
    rotZ_ = CreateSpin();
    scaleX_ = CreateSpin();
    scaleY_ = CreateSpin();
    scaleZ_ = CreateSpin();

    form->addRow("Pos X", posX_);
    form->addRow("Pos Y", posY_);
    form->addRow("Pos Z", posZ_);
    form->addRow("Rot X", rotX_);
    form->addRow("Rot Y", rotY_);
    form->addRow("Rot Z", rotZ_);
    form->addRow("Scale X", scaleX_);
    form->addRow("Scale Y", scaleY_);
    form->addRow("Scale Z", scaleZ_);

    auto* materialGroup = new QGroupBox("Material", root);
    auto* materialLayout = new QVBoxLayout(materialGroup);

    auto* colorRow = new QHBoxLayout();
    colorButton_ = new QPushButton("Base Color", materialGroup);
    colorRow->addWidget(colorButton_);

    auto* textureRow = new QHBoxLayout();
    texturePath_ = new QLineEdit(materialGroup);
    auto* browseButton = new QPushButton("Load Texture", materialGroup);
    textureRow->addWidget(texturePath_);
    textureRow->addWidget(browseButton);

    materialLayout->addLayout(colorRow);
    materialLayout->addLayout(textureRow);

    layout->addWidget(transformGroup);
    layout->addWidget(materialGroup);
    layout->addStretch();
    setWidget(root);

    connect(colorButton_, &QPushButton::clicked, this, [this]() { OnColorPick(); });
    connect(browseButton, &QPushButton::clicked, this, [this]() { OnTextureBrowse(); });

    auto onEdited = [this]() { if (onEdited_) { onEdited_(); } };
    connect(posX_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(posY_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(posZ_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(rotX_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(rotY_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(rotZ_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(scaleX_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(scaleY_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
    connect(scaleZ_, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onEdited);
}

void Dock_Properties::SetSelection(const Scene& scene, const Selection& selection)
{
    if (!selection.HasSelection())
    {
        return;
    }
    Entity* entity = scene.Find(selection.Selected().value());
    if (!entity)
    {
        return;
    }

    SetSpinValues(entity->Transform());
    baseColor_ = QColor::fromRgbF(entity->Material().baseColor.r, entity->Material().baseColor.g, entity->Material().baseColor.b, entity->Material().baseColor.a);
    texturePath_->setText(QString::fromStdString(entity->Material().baseColorTexture));
}

void Dock_Properties::ApplyToEntity(Entity& entity)
{
    entity.Transform().position = glm::vec3(posX_->value(), posY_->value(), posZ_->value());
    entity.Transform().rotation = glm::vec3(rotX_->value(), rotY_->value(), rotZ_->value());
    entity.Transform().scale = glm::vec3(scaleX_->value(), scaleY_->value(), scaleZ_->value());

    entity.Material().baseColor = glm::vec4(baseColor_.redF(), baseColor_.greenF(), baseColor_.blueF(), baseColor_.alphaF());
    entity.Material().baseColorTexture = texturePath_->text().toStdString();
}

void Dock_Properties::SetOnEdited(const std::function<void()>& callback)
{
    onEdited_ = callback;
}

void Dock_Properties::OnColorPick()
{
    QColor color = QColorDialog::getColor(baseColor_, this, "Select Base Color");
    if (color.isValid())
    {
        baseColor_ = color;
        if (onEdited_)
        {
            onEdited_();
        }
    }
}

void Dock_Properties::OnTextureBrowse()
{
    QString path = QFileDialog::getOpenFileName(this, "Select Texture", QString(), "Images (*.png *.jpg *.jpeg)");
    if (!path.isEmpty())
    {
        texturePath_->setText(path);
        if (onEdited_)
        {
            onEdited_();
        }
    }
}

void Dock_Properties::SetSpinValues(const TransformComponent& transform)
{
    posX_->setValue(transform.position.x);
    posY_->setValue(transform.position.y);
    posZ_->setValue(transform.position.z);
    rotX_->setValue(transform.rotation.x);
    rotY_->setValue(transform.rotation.y);
    rotZ_->setValue(transform.rotation.z);
    scaleX_->setValue(transform.scale.x);
    scaleY_->setValue(transform.scale.y);
    scaleZ_->setValue(transform.scale.z);
}
}

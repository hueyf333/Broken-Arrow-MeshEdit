// File: MeshEditor/src/ui/Dock_Properties.cpp
#include "ui/Dock_Properties.h"

#include <QFormLayout>
#include <QFileDialog>
#include <QGroupBox>
#include <glm/glm.hpp>

namespace MeshEditor
{
    static QDoubleSpinBox* MakeSpinBox(QWidget* parent, double min, double max)
    {
        auto* spin = new QDoubleSpinBox(parent);
        spin->setRange(min, max);
        spin->setDecimals(3);
        spin->setSingleStep(0.1);
        return spin;
    }

    Dock_Properties::Dock_Properties(App& app, QWidget* parent)
        : QDockWidget("Properties", parent), mApp(app)
    {
        QWidget* container = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(container);

        QGroupBox* transformGroup = new QGroupBox("Transform", container);
        QFormLayout* transformLayout = new QFormLayout(transformGroup);

        mPosX = MakeSpinBox(transformGroup, -1000, 1000);
        mPosY = MakeSpinBox(transformGroup, -1000, 1000);
        mPosZ = MakeSpinBox(transformGroup, -1000, 1000);
        mRotX = MakeSpinBox(transformGroup, -6.28, 6.28);
        mRotY = MakeSpinBox(transformGroup, -6.28, 6.28);
        mRotZ = MakeSpinBox(transformGroup, -6.28, 6.28);
        mScaleX = MakeSpinBox(transformGroup, 0.01, 1000);
        mScaleY = MakeSpinBox(transformGroup, 0.01, 1000);
        mScaleZ = MakeSpinBox(transformGroup, 0.01, 1000);

        transformLayout->addRow("Position X", mPosX);
        transformLayout->addRow("Position Y", mPosY);
        transformLayout->addRow("Position Z", mPosZ);
        transformLayout->addRow("Rotation X", mRotX);
        transformLayout->addRow("Rotation Y", mRotY);
        transformLayout->addRow("Rotation Z", mRotZ);
        transformLayout->addRow("Scale X", mScaleX);
        transformLayout->addRow("Scale Y", mScaleY);
        transformLayout->addRow("Scale Z", mScaleZ);
        transformGroup->setLayout(transformLayout);

        QGroupBox* materialGroup = new QGroupBox("Material", container);
        QFormLayout* materialLayout = new QFormLayout(materialGroup);
        mColorButton = new QPushButton("Pick Base Color", materialGroup);
        mTexturePath = new QLineEdit(materialGroup);
        mLoadTextureButton = new QPushButton("Load Texture", materialGroup);
        materialLayout->addRow("Base Color", mColorButton);
        materialLayout->addRow("Texture", mTexturePath);
        materialLayout->addRow("", mLoadTextureButton);
        materialGroup->setLayout(materialLayout);

        layout->addWidget(transformGroup);
        layout->addWidget(materialGroup);
        layout->addStretch();
        container->setLayout(layout);
        setWidget(container);

        auto onValueChanged = [this]() { ApplyTransform(); };
        connect(mPosX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mPosY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mPosZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mRotX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mRotY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mRotZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mScaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mScaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);
        connect(mScaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, onValueChanged);

        connect(mColorButton, &QPushButton::clicked, this, [this]()
        {
            if (!mApp.GetSelection().HasSelection())
            {
                return;
            }
            QColor color = QColorDialog::getColor(Qt::white, this, "Select Base Color");
            if (!color.isValid())
            {
                return;
            }
            ApplyMaterial();
            auto* entity = mApp.GetScene().FindEntity(mApp.GetSelection().Selected());
            if (entity)
            {
                entity->GetMaterial().baseColor = glm::vec4(color.redF(), color.greenF(), color.blueF(), 1.0f);
            }
        });

        connect(mLoadTextureButton, &QPushButton::clicked, this, [this]()
        {
            if (!mApp.GetSelection().HasSelection())
            {
                return;
            }
            QString file = QFileDialog::getOpenFileName(this, "Load Texture", QString(), "Images (*.png *.jpg *.jpeg)");
            if (file.isEmpty())
            {
                return;
            }
            mTexturePath->setText(file);
            ApplyMaterial();
        });

        connect(mTexturePath, &QLineEdit::editingFinished, this, [this]() { ApplyMaterial(); });
    }

    void Dock_Properties::ApplyTransform()
    {
        if (!mApp.GetSelection().HasSelection())
        {
            return;
        }
        auto* entity = mApp.GetScene().FindEntity(mApp.GetSelection().Selected());
        if (!entity)
        {
            return;
        }
        auto& transform = entity->GetTransform();
        transform.position = glm::vec3(mPosX->value(), mPosY->value(), mPosZ->value());
        transform.rotation = glm::vec3(mRotX->value(), mRotY->value(), mRotZ->value());
        transform.scale = glm::vec3(mScaleX->value(), mScaleY->value(), mScaleZ->value());
    }

    void Dock_Properties::ApplyMaterial()
    {
        if (!mApp.GetSelection().HasSelection())
        {
            return;
        }
        auto* entity = mApp.GetScene().FindEntity(mApp.GetSelection().Selected());
        if (!entity)
        {
            return;
        }
        entity->GetMaterial().baseColorTexturePath = mTexturePath->text().toStdString();
    }

    void Dock_Properties::Refresh()
    {
        auto* entity = mApp.GetScene().FindEntity(mApp.GetSelection().Selected());
        if (!entity)
        {
            return;
        }
        const auto& transform = entity->GetTransform();
        mPosX->setValue(transform.position.x);
        mPosY->setValue(transform.position.y);
        mPosZ->setValue(transform.position.z);
        mRotX->setValue(transform.rotation.x);
        mRotY->setValue(transform.rotation.y);
        mRotZ->setValue(transform.rotation.z);
        mScaleX->setValue(transform.scale.x);
        mScaleY->setValue(transform.scale.y);
        mScaleZ->setValue(transform.scale.z);
        mTexturePath->setText(QString::fromStdString(entity->GetMaterial().baseColorTexturePath));
    }
}

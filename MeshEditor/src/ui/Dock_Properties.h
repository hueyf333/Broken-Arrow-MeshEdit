// File: MeshEditor/src/ui/Dock_Properties.h
#pragma once

#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QColorDialog>

#include "app/App.h"

namespace MeshEditor
{
    class Dock_Properties : public QDockWidget
    {
    public:
        explicit Dock_Properties(App& app, QWidget* parent = nullptr);
        void Refresh();

    private:
        void ApplyTransform();
        void ApplyMaterial();

        App& mApp;
        QDoubleSpinBox* mPosX = nullptr;
        QDoubleSpinBox* mPosY = nullptr;
        QDoubleSpinBox* mPosZ = nullptr;
        QDoubleSpinBox* mRotX = nullptr;
        QDoubleSpinBox* mRotY = nullptr;
        QDoubleSpinBox* mRotZ = nullptr;
        QDoubleSpinBox* mScaleX = nullptr;
        QDoubleSpinBox* mScaleY = nullptr;
        QDoubleSpinBox* mScaleZ = nullptr;
        QLineEdit* mTexturePath = nullptr;
        QPushButton* mColorButton = nullptr;
        QPushButton* mLoadTextureButton = nullptr;
    };
}

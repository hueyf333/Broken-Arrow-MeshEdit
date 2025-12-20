// File: MeshEditor/src/ui/ViewportWidget.h
#pragma once

#include <QWidget>
#include <QTimer>

#include "app/App.h"
#include "render/RendererDX11.h"
#include "tools/Tool_Transform.h"
#include "ui/GizmoOverlay.h"

namespace MeshEditor
{
    class ViewportWidget : public QWidget
    {
    public:
        explicit ViewportWidget(App& app, QWidget* parent = nullptr);

        void BeginTransform(TransformMode mode);
        void UpdateTransform(TransformMode mode, const QPoint& delta);
        void EndTransform(TransformMode mode);
        void DrawGizmoOverlay(TransformMode mode, QPainter& painter);

    protected:
        void resizeEvent(QResizeEvent* event) override;
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void wheelEvent(QWheelEvent* event) override;
        void keyPressEvent(QKeyEvent* event) override;

    private:
        void RenderFrame();
        void SelectAt(const QPoint& pos);

        App& mApp;
        RendererDX11 mRenderer;
        QTimer mTimer;
        bool mInitialized = false;
        bool mOrbiting = false;
        bool mPanning = false;
        bool mDollying = false;
        QPoint mLastMouse;
        Transform mStartTransform {};
        GizmoOverlay mGizmo;
    };
}

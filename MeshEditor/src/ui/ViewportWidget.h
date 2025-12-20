// File: MeshEditor/src/ui/ViewportWidget.h
#pragma once

#include <QWidget>
#include <QTimer>
#include <functional>

#include "render/RendererDX11.h"
#include "scene/Scene.h"
#include "scene/Selection.h"
#include "tools/ToolManager.h"
#include "ui/GizmoOverlay.h"
#include "engine/Math.h"

namespace mesh
{
class ViewportWidget : public QWidget
{
public:
    ViewportWidget(Scene& scene, Selection& selection, ToolManager& toolManager, QWidget* parent = nullptr);

    ID3D11Device* GetDevice() const { return rendererDevice_; }
    RendererDX11& Renderer() { return renderer_; }
    void EnsureRenderer();
    void SetFpsCallback(const std::function<void(float)>& callback) { fpsCallback_ = callback; }
    void SetSelectionCallback(const std::function<void()>& callback) { selectionCallback_ = callback; }

    void SetWireframe(bool enabled);
    void SetShaded(bool enabled);
    QSize sizeHint() const override { return {800, 600}; }

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void InitializeRenderer();
    void OnRenderTick();
    void UpdateGizmoOverlay();
    void PerformSelection(const QPoint& pos);
    Ray ScreenPointToRay(const QPoint& pos) const;
    void NotifyFps(float fps);
    void NotifySelection();

    Scene& scene_;
    Selection& selection_;
    ToolManager& toolManager_;

    RendererDX11 renderer_;
    ID3D11Device* rendererDevice_ = nullptr;

    QTimer timer_;
    GizmoOverlay* gizmoOverlay_ = nullptr;

    std::function<void(float)> fpsCallback_;
    std::function<void()> selectionCallback_;

    QPoint lastMouse_;
    bool altDown_ = false;
    bool dragging_ = false;

    float fpsAccumulator_ = 0.0f;
    int fpsFrames_ = 0;
};
}

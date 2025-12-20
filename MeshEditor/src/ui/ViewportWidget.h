// File: MeshEditor/src/ui/ViewportWidget.h
#pragma once

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

namespace MeshEditor
{
class RendererDX11;
class Scene;
class ToolManager;
class UndoStack;

class ViewportWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewportWidget(QWidget* parent = nullptr);
    ~ViewportWidget() override;

    void SetScene(Scene* scene);
    void SetToolManager(ToolManager* manager);
    void SetUndoStack(UndoStack* undo);

    void ToggleWireframe();
    void ToggleShaded();
    float GetFPS() const;

public slots:
    void OnEntitySelected(int entityId);

signals:
    void SelectionChanged();

protected:
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void OnTick();

    RendererDX11* m_renderer;
    Scene* m_scene;
    ToolManager* m_tools;
    UndoStack* m_undo;
    QTimer m_timer;
    QElapsedTimer m_fpsTimer;
    int m_frameCount;
    float m_fps;
};
}

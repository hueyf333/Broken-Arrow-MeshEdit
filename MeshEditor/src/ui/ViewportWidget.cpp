// File: MeshEditor/src/ui/ViewportWidget.cpp
#include "ViewportWidget.h"
#include "render/RendererDX11.h"
#include "scene/Scene.h"
#include "tools/ToolManager.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <windows.h>

namespace MeshEditor
{
ViewportWidget::ViewportWidget(QWidget* parent)
    : QWidget(parent)
    , m_renderer(new RendererDX11())
    , m_scene(nullptr)
    , m_tools(nullptr)
    , m_undo(nullptr)
    , m_frameCount(0)
    , m_fps(0.0f)
{
    setAttribute(Qt::WA_NativeWindow);
    setFocusPolicy(Qt::StrongFocus);

    m_timer.setInterval(16);
    connect(&m_timer, &QTimer::timeout, this, &ViewportWidget::OnTick);
    m_fpsTimer.start();
}

ViewportWidget::~ViewportWidget()
{
    delete m_renderer;
}

void ViewportWidget::SetScene(Scene* scene)
{
    m_scene = scene;
    m_renderer->SetScene(scene);
}

void ViewportWidget::SetToolManager(ToolManager* manager)
{
    m_tools = manager;
    m_renderer->SetToolManager(manager);
}

void ViewportWidget::SetUndoStack(UndoStack* undo)
{
    m_undo = undo;
    if (m_tools)
        m_tools->BindUndoStack(undo);
}

void ViewportWidget::ToggleWireframe()
{
    m_renderer->ToggleWireframe();
}

void ViewportWidget::ToggleShaded()
{
    m_renderer->ToggleShaded();
}

float ViewportWidget::GetFPS() const
{
    return m_fps;
}

void ViewportWidget::OnEntitySelected(int entityId)
{
    if (m_scene)
    {
        m_scene->GetSelection().SetPrimary(entityId, *m_scene);
        emit SelectionChanged();
    }
}

void ViewportWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!m_renderer->IsInitialized())
    {
        m_renderer->Initialize(reinterpret_cast<HWND>(winId()), width(), height());
        m_timer.start();
    }
}

void ViewportWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_renderer->IsInitialized())
    {
        m_renderer->Resize(width(), height());
    }
}

void ViewportWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_tools)
        m_tools->OnMousePress(event, m_scene);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_tools)
        m_tools->OnMouseMove(event, m_scene);
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (m_tools)
        m_tools->OnMouseRelease(event, m_scene);
}

void ViewportWidget::keyPressEvent(QKeyEvent* event)
{
    if (m_renderer)
        m_renderer->OnKeyPress(event);
}

void ViewportWidget::OnTick()
{
    if (!m_renderer->IsInitialized())
        return;

    m_renderer->Render();

    m_frameCount++;
    if (m_fpsTimer.elapsed() > 500)
    {
        m_fps = m_frameCount * 1000.0f / m_fpsTimer.elapsed();
        m_frameCount = 0;
        m_fpsTimer.restart();
    }
}
}

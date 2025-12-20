// File: MeshEditor/src/ui/ViewportWidget.cpp
#include "ViewportWidget.h"

#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>

#include "engine/Time.h"
#include "engine/Math.h"
#include "render/RenderMesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace mesh
{
ViewportWidget::ViewportWidget(Scene& scene, Selection& selection, ToolManager& toolManager, QWidget* parent)
    : QWidget(parent), scene_(scene), selection_(selection), toolManager_(toolManager)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);

    gizmoOverlay_ = new GizmoOverlay(this);
    gizmoOverlay_->resize(size());
    gizmoOverlay_->show();

    connect(&timer_, &QTimer::timeout, this, &ViewportWidget::OnRenderTick);
    timer_.start(16);
}

void ViewportWidget::InitializeRenderer()
{
    if (rendererDevice_)
    {
        return;
    }
    HWND hwnd = reinterpret_cast<HWND>(winId());
    renderer_.Initialize(hwnd, width(), height());
    rendererDevice_ = renderer_.GetDevice();
}

void ViewportWidget::EnsureRenderer()
{
    InitializeRenderer();
}

void ViewportWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    InitializeRenderer();
}

void ViewportWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (rendererDevice_)
    {
        renderer_.Resize(event->size().width(), event->size().height());
    }
    if (gizmoOverlay_)
    {
        gizmoOverlay_->resize(event->size());
    }
}

void ViewportWidget::mousePressEvent(QMouseEvent* event)
{
    setFocus();
    InitializeRenderer();
    lastMouse_ = event->pos();
    altDown_ = event->modifiers().testFlag(Qt::AltModifier);

    if (!altDown_ && event->button() == Qt::LeftButton)
    {
        PerformSelection(event->pos());
    }

    toolManager_.OnMousePress(event);
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - lastMouse_;
    lastMouse_ = event->pos();

    if (altDown_)
    {
        if (event->buttons().testFlag(Qt::LeftButton))
        {
            renderer_.GetCamera().Orbit(delta.x(), delta.y());
        }
        else if (event->buttons().testFlag(Qt::MiddleButton))
        {
            renderer_.GetCamera().Pan(delta.x(), -delta.y());
        }
        else if (event->buttons().testFlag(Qt::RightButton))
        {
            renderer_.GetCamera().Dolly(delta.x() + delta.y());
        }
    }

    toolManager_.OnMouseMove(event);
    UpdateGizmoOverlay();
}

void ViewportWidget::mouseReleaseEvent(QMouseEvent* event)
{
    toolManager_.OnMouseRelease(event);
}

void ViewportWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_F && selection_.HasSelection())
    {
        Entity* entity = scene_.Find(selection_.Selected().value());
        if (entity && entity->Mesh().mesh)
        {
            glm::vec3 min = entity->Mesh().mesh->BoundsMin();
            glm::vec3 max = entity->Mesh().mesh->BoundsMax();
            glm::vec3 center = (min + max) * 0.5f + entity->Transform().position;
            float radius = glm::length(max - min) * 0.5f;
            renderer_.GetCamera().Frame(center, radius);
        }
    }
    QWidget::keyPressEvent(event);
}

void ViewportWidget::SetWireframe(bool enabled)
{
    renderer_.SetWireframe(enabled);
}

void ViewportWidget::SetShaded(bool enabled)
{
    renderer_.SetShaded(enabled);
}

void ViewportWidget::OnRenderTick()
{
    InitializeRenderer();
    Time::Tick();
    renderer_.Render(scene_, selection_);
    UpdateGizmoOverlay();

    fpsAccumulator_ += Time::DeltaSeconds();
    ++fpsFrames_;
    if (fpsAccumulator_ >= 1.0f)
    {
        float fps = fpsFrames_ / fpsAccumulator_;
        NotifyFps(fps);
        fpsAccumulator_ = 0.0f;
        fpsFrames_ = 0;
    }
}

void ViewportWidget::UpdateGizmoOverlay()
{
    if (!gizmoOverlay_)
    {
        return;
    }
    gizmoOverlay_->SetVisibleAxis(true, true, true);
    gizmoOverlay_->SetOrigin(rect().center());
}

void ViewportWidget::PerformSelection(const QPoint& pos)
{
    Ray ray = ScreenPointToRay(pos);
    float closest = 1e9f;
    EntityId hitId = 0;

    for (const auto& entity : scene_.Entities())
    {
        if (!entity->Mesh().mesh)
        {
            continue;
        }
        glm::vec3 min = entity->Mesh().mesh->BoundsMin();
        glm::vec3 max = entity->Mesh().mesh->BoundsMax();
        glm::vec3 worldMin = min * entity->Transform().scale + entity->Transform().position;
        glm::vec3 worldMax = max * entity->Transform().scale + entity->Transform().position;

        float t = 0.0f;
        if (RayIntersectsAabb(ray, worldMin, worldMax, t))
        {
            if (t < closest)
            {
                closest = t;
                hitId = entity->Id();
            }
        }
    }

    if (hitId != 0)
    {
        selection_.Set(hitId);
    }
    else
    {
        selection_.Clear();
    }
    NotifySelection();
}

void ViewportWidget::NotifyFps(float fps)
{
    if (fpsCallback_)
    {
        fpsCallback_(fps);
    }
}

void ViewportWidget::NotifySelection()
{
    if (selectionCallback_)
    {
        selectionCallback_();
    }
}

Ray ViewportWidget::ScreenPointToRay(const QPoint& pos) const
{
    float ndcX = (2.0f * pos.x()) / width() - 1.0f;
    float ndcY = 1.0f - (2.0f * pos.y()) / height();

    glm::mat4 projection = renderer_.GetCamera().ProjectionMatrix();
    glm::mat4 view = renderer_.GetCamera().ViewMatrix();
    glm::mat4 inv = glm::inverse(projection * view);

    glm::vec4 nearPoint = inv * glm::vec4(ndcX, ndcY, 0.0f, 1.0f);
    glm::vec4 farPoint = inv * glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    Ray ray;
    ray.origin = glm::vec3(nearPoint);
    ray.direction = glm::normalize(glm::vec3(farPoint - nearPoint));
    return ray;
}
}

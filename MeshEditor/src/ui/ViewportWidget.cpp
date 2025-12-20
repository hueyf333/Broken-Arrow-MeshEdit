// File: MeshEditor/src/ui/ViewportWidget.cpp
#include "ui/ViewportWidget.h"

#include "engine/Command.h"
#include "engine/Logger.h"
#include "engine/Time.h"
#include "scene/Entity.h"

#include <QMouseEvent>
#include <QPainter>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <cfloat>
#include <limits>

namespace MeshEditor
{
    ViewportWidget::ViewportWidget(App& app, QWidget* parent)
        : QWidget(parent), mApp(app)
    {
        setAttribute(Qt::WA_NativeWindow);
        setAttribute(Qt::WA_PaintOnScreen, true);
        setFocusPolicy(Qt::StrongFocus);

        connect(&mTimer, &QTimer::timeout, this, [this]() { RenderFrame(); });
        mTimer.start(16);
    }

    void ViewportWidget::resizeEvent(QResizeEvent* event)
    {
        QWidget::resizeEvent(event);
        if (!mInitialized)
        {
            HWND hwnd = reinterpret_cast<HWND>(winId());
            mInitialized = mRenderer.Initialize(hwnd, width(), height());
        }
        else
        {
            mRenderer.Resize(width(), height());
        }
    }

    void ViewportWidget::paintEvent(QPaintEvent* event)
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        auto* tool = mApp.GetToolManager().ActiveTool();
        if (tool)
        {
            tool->DrawOverlay(this, painter);
        }
    }

    void ViewportWidget::RenderFrame()
    {
        if (!mInitialized)
        {
            return;
        }
        Time::Tick();
        mRenderer.Render(mApp.GetScene(), mApp.GetSelection());
        update();
    }

    void ViewportWidget::mousePressEvent(QMouseEvent* event)
    {
        if (event->modifiers() & Qt::AltModifier)
        {
            if (event->button() == Qt::LeftButton)
            {
                mOrbiting = true;
            }
            else if (event->button() == Qt::MiddleButton)
            {
                mPanning = true;
            }
            else if (event->button() == Qt::RightButton)
            {
                mDollying = true;
            }
        }
        else
        {
            auto* tool = mApp.GetToolManager().ActiveTool();
            if (tool)
            {
                tool->OnMousePress(this, event);
            }
            if (event->button() == Qt::LeftButton)
            {
                SelectAt(event->pos());
            }
        }

        mLastMouse = event->pos();
    }

    void ViewportWidget::mouseMoveEvent(QMouseEvent* event)
    {
        QPoint delta = event->pos() - mLastMouse;
        if (mOrbiting)
        {
            mRenderer.GetCamera().Orbit(glm::vec2(delta.x() * 0.005f, delta.y() * 0.005f));
        }
        else if (mPanning)
        {
            mRenderer.GetCamera().Pan(glm::vec2(delta.x() * 0.01f, -delta.y() * 0.01f));
        }
        else if (mDollying)
        {
            mRenderer.GetCamera().Dolly(delta.y() * 0.02f);
        }
        else
        {
            auto* tool = mApp.GetToolManager().ActiveTool();
            if (tool)
            {
                tool->OnMouseMove(this, event);
            }
        }
        mLastMouse = event->pos();
    }

    void ViewportWidget::mouseReleaseEvent(QMouseEvent* event)
    {
        mOrbiting = false;
        mPanning = false;
        mDollying = false;

        auto* tool = mApp.GetToolManager().ActiveTool();
        if (tool)
        {
            tool->OnMouseRelease(this, event);
        }
    }

    void ViewportWidget::wheelEvent(QWheelEvent* event)
    {
        mRenderer.GetCamera().Dolly(-event->angleDelta().y() * 0.001f);
    }

    void ViewportWidget::keyPressEvent(QKeyEvent* event)
    {
        if (event->key() == Qt::Key_F)
        {
            auto* entity = mApp.GetScene().FindEntity(mApp.GetSelection().Selected());
            if (entity)
            {
                mRenderer.GetCamera().SetTarget(entity->GetTransform().position);
            }
        }
    }

    void ViewportWidget::SelectAt(const QPoint& pos)
    {
        if (mApp.GetScene().Entities().empty())
        {
            return;
        }

        float x = (2.0f * pos.x()) / width() - 1.0f;
        float y = 1.0f - (2.0f * pos.y()) / height();
        glm::vec4 rayClip(x, y, -1.0f, 1.0f);
        glm::mat4 invViewProj = glm::inverse(mRenderer.GetCamera().ViewProjection());
        glm::vec4 rayWorld = invViewProj * rayClip;
        rayWorld /= rayWorld.w;
        glm::vec3 origin = mRenderer.GetCamera().Position();
        glm::vec3 dir = glm::normalize(glm::vec3(rayWorld) - origin);

        float nearest = std::numeric_limits<float>::max();
        uint64_t hitId = 0;
        for (const auto& entityPtr : mApp.GetScene().Entities())
        {
            const auto& mesh = entityPtr->GetMesh();
            if (mesh.positions.empty())
            {
                continue;
            }
            glm::vec3 min(FLT_MAX);
            glm::vec3 max(-FLT_MAX);
            for (const auto& p : mesh.positions)
            {
                glm::vec3 worldPos = p + entityPtr->GetTransform().position;
                min = glm::min(min, worldPos);
                max = glm::max(max, worldPos);
            }

            float dx = (std::abs(dir.x) < 1e-5f) ? 1e-5f : dir.x;
            float dy = (std::abs(dir.y) < 1e-5f) ? 1e-5f : dir.y;
            float dz = (std::abs(dir.z) < 1e-5f) ? 1e-5f : dir.z;

            float tmin = (min.x - origin.x) / dx;
            float tmax = (max.x - origin.x) / dx;
            if (tmin > tmax) std::swap(tmin, tmax);
            float tymin = (min.y - origin.y) / dy;
            float tymax = (max.y - origin.y) / dy;
            if (tymin > tymax) std::swap(tymin, tymax);
            if ((tmin > tymax) || (tymin > tmax))
            {
                continue;
            }
            if (tymin > tmin) tmin = tymin;
            if (tymax < tmax) tmax = tymax;

            float tzmin = (min.z - origin.z) / dz;
            float tzmax = (max.z - origin.z) / dz;
            if (tzmin > tzmax) std::swap(tzmin, tzmax);
            if ((tmin > tzmax) || (tzmin > tmax))
            {
                continue;
            }
            if (tzmin > tmin) tmin = tzmin;

            if (tmin < nearest && tmin > 0)
            {
                nearest = tmin;
                hitId = entityPtr->Id();
            }
        }

        if (hitId != 0)
        {
            mApp.GetSelection().SetSelected(hitId);
        }
    }

    void ViewportWidget::BeginTransform(TransformMode mode)
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
        mStartTransform = entity->GetTransform();
    }

    void ViewportWidget::UpdateTransform(TransformMode mode, const QPoint& delta)
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
        if (mode == TransformMode::Move)
        {
            transform.position.x = mStartTransform.position.x + delta.x() * 0.01f;
        }
        else
        {
            // TODO: Implement rotate/scale.
        }
    }

    void ViewportWidget::EndTransform(TransformMode mode)
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
        Transform endTransform = entity->GetTransform();
        if (mode != TransformMode::Move)
        {
            return;
        }

        uint64_t id = entity->Id();
        mApp.GetUndoStack().Push(Command("Move", [this, id, endTransform]()
        {
            if (auto* e = mApp.GetScene().FindEntity(id))
            {
                e->GetTransform() = endTransform;
            }
        }, [this, id, start = mStartTransform]()
        {
            if (auto* e = mApp.GetScene().FindEntity(id))
            {
                e->GetTransform() = start;
            }
        }));
    }

    void ViewportWidget::DrawGizmoOverlay(TransformMode mode, QPainter& painter)
    {
        if (!mApp.GetSelection().HasSelection())
        {
            return;
        }
        QPoint center(width() / 2, height() / 2);
        mGizmo.DrawAxis(painter, center, 40);
        if (mode == TransformMode::Move)
        {
            painter.setPen(QPen(Qt::red, 2));
            painter.drawText(center + QPoint(45, 0), "X");
        }
    }
}

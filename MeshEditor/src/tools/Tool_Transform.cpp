// File: MeshEditor/src/tools/Tool_Transform.cpp
#include "Tool_Transform.h"

#include <glm/glm.hpp>

namespace mesh
{
Tool_Transform::Tool_Transform(Scene& scene, Selection& selection, ViewportWidget& viewport, UndoStack& undoStack, ToolType type)
    : scene_(scene), selection_(selection), viewport_(viewport), undoStack_(undoStack), type_(type)
{
}

QString Tool_Transform::Name() const
{
    switch (type_)
    {
    case ToolType::Move:
        return QStringLiteral("Move");
    case ToolType::Rotate:
        return QStringLiteral("Rotate");
    case ToolType::Scale:
        return QStringLiteral("Scale");
    default:
        return QStringLiteral("Transform");
    }
}

void Tool_Transform::OnMousePress(QMouseEvent* event)
{
    if (type_ != ToolType::Move)
    {
        return;
    }
    if (event->button() == Qt::LeftButton && selection_.HasSelection())
    {
        Entity* entity = scene_.Find(selection_.Selected().value());
        if (!entity)
        {
            return;
        }
        dragging_ = true;
        dragStart_ = event->pos();
        startPosition_ = entity->Transform().position;
    }
}

void Tool_Transform::OnMouseMove(QMouseEvent* event)
{
    if (!dragging_ || type_ != ToolType::Move)
    {
        return;
    }

    Entity* entity = scene_.Find(selection_.Selected().value());
    if (!entity)
    {
        return;
    }

    int dx = event->pos().x() - dragStart_.x();
    float delta = static_cast<float>(dx) * 0.01f;
    entity->Transform().position = startPosition_ + glm::vec3(delta, 0.0f, 0.0f);
}

void Tool_Transform::OnMouseRelease(QMouseEvent*)
{
    if (!dragging_)
    {
        return;
    }
    dragging_ = false;

    if (!selection_.HasSelection() || type_ != ToolType::Move)
    {
        return;
    }

    Entity* entity = scene_.Find(selection_.Selected().value());
    if (!entity)
    {
        return;
    }

    glm::vec3 endPosition = entity->Transform().position;
    if (endPosition == startPosition_)
    {
        return;
    }

    auto before = startPosition_;
    auto after = endPosition;
    undoStack_.Execute(std::make_unique<FunctionCommand>(
        "Move",
        [entity, after]() { entity->Transform().position = after; },
        [entity, before]() { entity->Transform().position = before; }));
}
}

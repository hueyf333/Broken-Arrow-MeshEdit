// File: MeshEditor/src/tools/Tool_Transform.cpp
#include "Tool_Transform.h"
#include "scene/Scene.h"
#include "scene/Selection.h"

namespace MeshEditor
{
Tool_Transform::Tool_Transform(Mode mode)
    : m_mode(mode)
    , m_dragging(false)
{
}

std::string Tool_Transform::GetName() const
{
    switch (m_mode)
    {
    case Mode::Move: return "Move";
    case Mode::Rotate: return "Rotate";
    case Mode::Scale: return "Scale";
    }
    return "Transform";
}

void Tool_Transform::OnMousePress(QMouseEvent* event, Scene* scene)
{
    if (!scene || !event)
        return;

    m_dragging = true;
    m_lastPos = event->pos();
}

void Tool_Transform::OnMouseMove(QMouseEvent* event, Scene* scene)
{
    if (!scene || !event || !m_dragging)
        return;

    auto* entity = scene->GetSelection().GetPrimary();
    if (!entity)
        return;

    QPoint delta = event->pos() - m_lastPos;
    m_lastPos = event->pos();

    auto transform = entity->GetTransform();
    if (m_mode == Mode::Move)
    {
        transform.position.x += delta.x() * 0.01f;
    }
    // TODO: Rotate and Scale.

    entity->SetTransform(transform);
}

void Tool_Transform::OnMouseRelease(QMouseEvent*, Scene*)
{
    m_dragging = false;
}
}

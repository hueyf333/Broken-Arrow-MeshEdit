// File: MeshEditor/src/engine/Command.cpp
#include "Command.h"
#include "scene/Scene.h"
#include "scene/Entity.h"

namespace MeshEditor
{
TransformCommand::TransformCommand(int entityId, const Transform& before, const Transform& after, Scene* scene)
    : m_entityId(entityId)
    , m_before(before)
    , m_after(after)
    , m_scene(scene)
{
}

void TransformCommand::Execute()
{
    if (auto* entity = m_scene->FindEntity(m_entityId))
    {
        entity->SetTransform(m_after);
    }
}

void TransformCommand::Undo()
{
    if (auto* entity = m_scene->FindEntity(m_entityId))
    {
        entity->SetTransform(m_before);
    }
}

ImportCommand::ImportCommand(int entityId, Scene* scene)
    : m_entityId(entityId)
    , m_scene(scene)
{
}

void ImportCommand::Execute()
{
    // Entity already added during import.
}

void ImportCommand::Undo()
{
    if (m_scene)
    {
        m_scene->RemoveEntity(m_entityId);
    }
}
}

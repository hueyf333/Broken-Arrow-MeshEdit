// File: MeshEditor/src/engine/Command.h
#pragma once

#include <memory>
#include "engine/Core.h"

namespace MeshEditor
{
class Scene;

class Command
{
public:
    virtual ~Command() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
};

class TransformCommand : public Command
{
public:
    TransformCommand(int entityId, const Transform& before, const Transform& after, Scene* scene);
    void Execute() override;
    void Undo() override;

private:
    int m_entityId;
    Transform m_before;
    Transform m_after;
    Scene* m_scene;
};

class ImportCommand : public Command
{
public:
    ImportCommand(int entityId, Scene* scene);
    void Execute() override;
    void Undo() override;

private:
    int m_entityId;
    Scene* m_scene;
};
}

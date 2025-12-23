#pragma once
#include "Core/Scene.hpp"
#include "Core/Mesh.hpp"
#include <memory>
#include <vector>
#include <string>

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string getName() const = 0;
};

class TransformCommand : public Command {
public:
    TransformCommand(SceneObject* obj, const glm::vec3& newPos, const glm::vec3& newRot, const glm::vec3& newScale);
    void execute() override;
    void undo() override;
    std::string getName() const override { return "Transform"; }
    
private:
    SceneObject* m_object;
    glm::vec3 m_oldPos, m_oldRot, m_oldScale;
    glm::vec3 m_newPos, m_newRot, m_newScale;
};

class MeshEditCommand : public Command {
public:
    MeshEditCommand(SceneObject* obj, const std::string& opName);
    void execute() override;
    void undo() override;
    std::string getName() const override { return m_opName; }
    
    void saveMeshState(); // Call before operation
    
private:
    SceneObject* m_object;
    std::string m_opName;
    Mesh m_oldMesh;
    Mesh m_newMesh;
    bool m_executed = false;
};

class CommandHistory {
public:
    void execute(std::unique_ptr<Command> cmd);
    void undo();
    void redo();
    bool canUndo() const { return m_currentIndex > 0; }
    bool canRedo() const { return m_currentIndex < static_cast<int>(m_commands.size()); }
    void clear();
    
private:
    std::vector<std::unique_ptr<Command>> m_commands;
    int m_currentIndex = 0;
};

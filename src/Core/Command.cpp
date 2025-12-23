#include "Core/Command.hpp"

TransformCommand::TransformCommand(SceneObject* obj, const glm::vec3& newPos, const glm::vec3& newRot, const glm::vec3& newScale)
    : m_object(obj)
    , m_oldPos(obj->position)
    , m_oldRot(obj->rotation)
    , m_oldScale(obj->scale)
    , m_newPos(newPos)
    , m_newRot(newRot)
    , m_newScale(newScale)
{
}

void TransformCommand::execute() {
    m_object->position = m_newPos;
    m_object->rotation = m_newRot;
    m_object->scale = m_newScale;
}

void TransformCommand::undo() {
    m_object->position = m_oldPos;
    m_object->rotation = m_oldRot;
    m_object->scale = m_oldScale;
}

MeshEditCommand::MeshEditCommand(SceneObject* obj, const std::string& opName)
    : m_object(obj)
    , m_opName(opName)
{
}

void MeshEditCommand::saveMeshState() {
    m_oldMesh = m_object->mesh;
}

void MeshEditCommand::execute() {
    if (!m_executed) {
        m_newMesh = m_object->mesh;
        m_executed = true;
    } else {
        m_object->mesh.freeGPU();
        m_object->mesh = m_newMesh;
        m_object->mesh.uploadToGPU();
    }
}

void MeshEditCommand::undo() {
    m_object->mesh.freeGPU();
    m_object->mesh = m_oldMesh;
    m_object->mesh.uploadToGPU();
}

void CommandHistory::execute(std::unique_ptr<Command> cmd) {
    // Remove any commands after current index
    if (m_currentIndex < static_cast<int>(m_commands.size())) {
        m_commands.erase(m_commands.begin() + m_currentIndex, m_commands.end());
    }
    
    cmd->execute();
    m_commands.push_back(std::move(cmd));
    m_currentIndex++;
}

void CommandHistory::undo() {
    if (canUndo()) {
        m_currentIndex--;
        m_commands[m_currentIndex]->undo();
    }
}

void CommandHistory::redo() {
    if (canRedo()) {
        m_commands[m_currentIndex]->execute();
        m_currentIndex++;
    }
}

void CommandHistory::clear() {
    m_commands.clear();
    m_currentIndex = 0;
}

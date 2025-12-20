// File: MeshEditor/src/engine/UndoStack.cpp
#include "UndoStack.h"

namespace MeshEditor
{
void UndoStack::Execute(std::unique_ptr<Command> command)
{
    if (!command)
        return;

    if (m_index + 1 < static_cast<int>(m_commands.size()))
    {
        m_commands.erase(m_commands.begin() + m_index + 1, m_commands.end());
    }
    command->Execute();
    m_commands.push_back(std::move(command));
    m_index = static_cast<int>(m_commands.size()) - 1;
}

void UndoStack::Undo()
{
    if (m_index < 0)
        return;

    m_commands[m_index]->Undo();
    m_index--;
}

void UndoStack::Redo()
{
    if (m_index + 1 >= static_cast<int>(m_commands.size()))
        return;

    m_index++;
    m_commands[m_index]->Execute();
}
}

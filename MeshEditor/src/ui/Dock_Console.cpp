// File: MeshEditor/src/ui/Dock_Console.cpp
#include "Dock_Console.h"
#include "engine/Logger.h"

namespace MeshEditor
{
Dock_Console::Dock_Console(QWidget* parent)
    : QDockWidget("Console", parent)
    , m_text(new QTextEdit(this))
{
    m_text->setReadOnly(true);
    setWidget(m_text);
}

void Dock_Console::BindLogger()
{
    Logger::SetCallback([this](const std::string& msg) {
        AppendLog(QString::fromStdString(msg));
    });
}

void Dock_Console::AppendLog(const QString& message)
{
    m_text->append(message);
}
}

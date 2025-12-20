// File: MeshEditor/src/ui/Dock_Console.cpp
#include "ui/Dock_Console.h"

#include <QVBoxLayout>

namespace MeshEditor
{
    Dock_Console::Dock_Console(QWidget* parent)
        : QDockWidget("Console", parent)
    {
        QWidget* container = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(container);
        mConsole = new QPlainTextEdit(container);
        mConsole->setReadOnly(true);
        layout->addWidget(mConsole);
        container->setLayout(layout);
        setWidget(container);
    }

    void Dock_Console::AppendLine(const QString& line)
    {
        mConsole->appendPlainText(line);
    }
}

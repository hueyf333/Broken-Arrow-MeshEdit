// File: MeshEditor/src/ui/Dock_Console.h
#pragma once

#include <QDockWidget>
#include <QPlainTextEdit>

namespace MeshEditor
{
    class Dock_Console : public QDockWidget
    {
    public:
        explicit Dock_Console(QWidget* parent = nullptr);
        void AppendLine(const QString& line);

    private:
        QPlainTextEdit* mConsole = nullptr;
    };
}

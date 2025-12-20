// File: MeshEditor/src/ui/Dock_Console.h
#pragma once

#include <QDockWidget>
#include <QTextEdit>

namespace mesh
{
class Dock_Console : public QDockWidget
{
public:
    explicit Dock_Console(QWidget* parent = nullptr);
    QTextEdit* Console() const { return console_; }

private:
    QTextEdit* console_ = nullptr;
};
}

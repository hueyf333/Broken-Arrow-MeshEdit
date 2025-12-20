// File: MeshEditor/src/ui/Dock_Console.cpp
#include "Dock_Console.h"

namespace mesh
{
Dock_Console::Dock_Console(QWidget* parent)
    : QDockWidget("Console", parent)
{
    console_ = new QTextEdit(this);
    console_->setReadOnly(true);
    setWidget(console_);
}
}

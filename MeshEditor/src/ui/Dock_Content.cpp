// File: MeshEditor/src/ui/Dock_Content.cpp
#include "Dock_Content.h"

namespace mesh
{
Dock_Content::Dock_Content(QWidget* parent)
    : QDockWidget("Content", parent)
{
    list_ = new QListWidget(this);
    setWidget(list_);
}

void Dock_Content::AddAsset(const QString& name)
{
    list_->addItem(name);
}
}

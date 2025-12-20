// File: MeshEditor/src/ui/Dock_Content.cpp
#include "Dock_Content.h"
#include "scene/Scene.h"
#include "scene/Entity.h"

namespace MeshEditor
{
Dock_Content::Dock_Content(QWidget* parent)
    : QDockWidget("Content Browser", parent)
    , m_scene(nullptr)
    , m_list(new QListWidget(this))
{
    setWidget(m_list);
}

void Dock_Content::BindScene(Scene* scene)
{
    m_scene = scene;
    Refresh();
}

void Dock_Content::Refresh()
{
    if (!m_scene)
        return;

    m_list->clear();
    for (const auto& entity : m_scene->GetEntities())
    {
        m_list->addItem(QString::fromStdString(entity.GetName()));
    }
}
}

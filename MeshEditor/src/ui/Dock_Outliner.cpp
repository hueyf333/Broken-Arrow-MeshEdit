// File: MeshEditor/src/ui/Dock_Outliner.cpp
#include "Dock_Outliner.h"
#include "scene/Scene.h"
#include "scene/Entity.h"

namespace MeshEditor
{
Dock_Outliner::Dock_Outliner(QWidget* parent)
    : QDockWidget("Outliner", parent)
    , m_scene(nullptr)
    , m_tree(new QTreeWidget(this))
{
    m_tree->setHeaderLabel("Scene");
    setWidget(m_tree);
    connect(m_tree, &QTreeWidget::itemClicked, this, &Dock_Outliner::OnItemClicked);
}

void Dock_Outliner::BindScene(Scene* scene)
{
    m_scene = scene;
    Refresh();
}

void Dock_Outliner::Refresh()
{
    if (!m_scene)
        return;

    m_tree->clear();
    for (const auto& entity : m_scene->GetEntities())
    {
        auto* item = new QTreeWidgetItem();
        item->setText(0, QString::fromStdString(entity.GetName()));
        item->setData(0, Qt::UserRole, entity.GetId());
        m_tree->addTopLevelItem(item);
    }
}

void Dock_Outliner::OnItemClicked(QTreeWidgetItem* item, int)
{
    if (!item || !m_scene)
        return;

    int id = item->data(0, Qt::UserRole).toInt();
    emit EntitySelected(id);
}

void Dock_Outliner::OnSelectionChanged()
{
    Refresh();
}
}

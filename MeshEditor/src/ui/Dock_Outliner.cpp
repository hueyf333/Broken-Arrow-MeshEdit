// File: MeshEditor/src/ui/Dock_Outliner.cpp
#include "Dock_Outliner.h"

namespace mesh
{
Dock_Outliner::Dock_Outliner(QWidget* parent)
    : QDockWidget("Outliner", parent)
{
    tree_ = new QTreeWidget(this);
    tree_->setHeaderHidden(true);
    setWidget(tree_);
}

void Dock_Outliner::Rebuild(const Scene& scene, const Selection& selection)
{
    tree_->clear();
    for (const auto& entity : scene.Entities())
    {
        auto* item = new QTreeWidgetItem(tree_);
        item->setText(0, QString::fromStdString(entity->Name()));
        item->setData(0, Qt::UserRole, QVariant::fromValue<qulonglong>(entity->Id()));
        if (selection.HasSelection() && selection.Selected().value() == entity->Id())
        {
            tree_->setCurrentItem(item);
        }
    }
}

std::optional<EntityId> Dock_Outliner::SelectedEntity() const
{
    auto* item = tree_->currentItem();
    if (!item)
    {
        return std::nullopt;
    }
    EntityId id = static_cast<EntityId>(item->data(0, Qt::UserRole).toULongLong());
    return id;
}

}

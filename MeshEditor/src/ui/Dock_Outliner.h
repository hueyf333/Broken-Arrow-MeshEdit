// File: MeshEditor/src/ui/Dock_Outliner.h
#pragma once

#include <QDockWidget>
#include <QTreeWidget>
#include <optional>
#include "scene/Scene.h"
#include "scene/Selection.h"

namespace mesh
{
class Dock_Outliner : public QDockWidget
{
public:
    explicit Dock_Outliner(QWidget* parent = nullptr);

    void Rebuild(const Scene& scene, const Selection& selection);
    std::optional<EntityId> SelectedEntity() const;
    QTreeWidget* Tree() const { return tree_; }

private:
    QTreeWidget* tree_ = nullptr;
};
}

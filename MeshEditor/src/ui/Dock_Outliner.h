// File: MeshEditor/src/ui/Dock_Outliner.h
#pragma once

#include <QDockWidget>
#include <QTreeWidget>

namespace MeshEditor
{
class Scene;

class Dock_Outliner : public QDockWidget
{
    Q_OBJECT

public:
    explicit Dock_Outliner(QWidget* parent = nullptr);
    void BindScene(Scene* scene);
    void Refresh();

signals:
    void EntitySelected(int entityId);

public slots:
    void OnSelectionChanged();

private slots:
    void OnItemClicked(QTreeWidgetItem* item, int column);

private:
    Scene* m_scene;
    QTreeWidget* m_tree;
};
}

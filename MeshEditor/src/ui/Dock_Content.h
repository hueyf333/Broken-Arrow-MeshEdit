// File: MeshEditor/src/ui/Dock_Content.h
#pragma once

#include <QDockWidget>
#include <QListWidget>

namespace MeshEditor
{
class Scene;

class Dock_Content : public QDockWidget
{
    Q_OBJECT

public:
    explicit Dock_Content(QWidget* parent = nullptr);
    void BindScene(Scene* scene);
    void Refresh();

private:
    Scene* m_scene;
    QListWidget* m_list;
};
}

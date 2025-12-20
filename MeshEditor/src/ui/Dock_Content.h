// File: MeshEditor/src/ui/Dock_Content.h
#pragma once

#include <QDockWidget>
#include <QListWidget>

namespace mesh
{
class Dock_Content : public QDockWidget
{
public:
    explicit Dock_Content(QWidget* parent = nullptr);
    void AddAsset(const QString& name);

private:
    QListWidget* list_ = nullptr;
};
}

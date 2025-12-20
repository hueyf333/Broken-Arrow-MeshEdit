// File: MeshEditor/src/ui/Dock_Outliner.h
#pragma once

#include <QDockWidget>
#include <QTreeWidget>

#include "app/App.h"

namespace MeshEditor
{
    class Dock_Outliner : public QDockWidget
    {
    public:
        explicit Dock_Outliner(App& app, QWidget* parent = nullptr);
        void Refresh();

    private:
        App& mApp;
        QTreeWidget* mTree = nullptr;
    };
}

// File: MeshEditor/src/ui/Dock_Content.h
#pragma once

#include <QDockWidget>
#include <QListWidget>

#include "app/App.h"

namespace MeshEditor
{
    class Dock_Content : public QDockWidget
    {
    public:
        explicit Dock_Content(App& app, QWidget* parent = nullptr);
        void Refresh();

    private:
        App& mApp;
        QListWidget* mList = nullptr;
    };
}

// File: MeshEditor/src/ui/MainWindow.h
#pragma once

#include <QMainWindow>
#include <QLabel>

#include "app/App.h"
#include "ui/Dock_Outliner.h"
#include "ui/Dock_Properties.h"
#include "ui/Dock_Content.h"
#include "ui/Dock_Console.h"
#include "ui/ViewportWidget.h"

namespace MeshEditor
{
    class MainWindow : public QMainWindow
    {
    public:
        explicit MainWindow(App& app, QWidget* parent = nullptr);

    private:
        void BuildMenus();
        void BuildToolbar();
        void UpdateStatus();
        void ImportMesh();
        void ExportSelected();
        void ResetLayout();

        App& mApp;
        ViewportWidget* mViewport = nullptr;
        Dock_Outliner* mOutliner = nullptr;
        Dock_Properties* mProperties = nullptr;
        Dock_Content* mContent = nullptr;
        Dock_Console* mConsole = nullptr;
        QLabel* mStatusFps = nullptr;
        QLabel* mStatusSelection = nullptr;
        QLabel* mStatusTool = nullptr;
    };
}

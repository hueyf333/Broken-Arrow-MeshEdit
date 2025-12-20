// File: MeshEditor/src/ui/MainWindow.h
#pragma once

#include <QMainWindow>
#include <memory>

#include "scene/Scene.h"
#include "scene/Selection.h"
#include "engine/UndoStack.h"
#include "tools/ToolManager.h"

namespace mesh
{
class Dock_Outliner;
class Dock_Properties;
class Dock_Content;
class Dock_Console;
class ViewportWidget;

class MainWindow : public QMainWindow
{
public:
    MainWindow();

private:
    void OnImportMesh();
    void OnExportSelected();
    void OnUndo();
    void OnRedo();
    void OnResetLayout();
    void OnSelectTool();
    void OnMoveTool();
    void OnRotateTool();
    void OnScaleTool();
    void OnToggleWireframe(bool checked);
    void OnToggleShaded(bool checked);
    void OnOutlinerSelectionChanged();
    void OnViewportSelectionChanged();
    void OnPropertiesChanged();
    void OnViewportFps(float fps);

private:
    void CreateMenus();
    void CreateToolbar();
    void CreateDocks();
    void PopulateDefaultScene();
    void UpdateOutliner();
    void UpdateStatus();

    Scene scene_;
    Selection selection_;
    UndoStack undoStack_;
    ToolManager toolManager_;

    Dock_Outliner* outlinerDock_ = nullptr;
    Dock_Properties* propertiesDock_ = nullptr;
    Dock_Content* contentDock_ = nullptr;
    Dock_Console* consoleDock_ = nullptr;
    ViewportWidget* viewport_ = nullptr;

    float lastFps_ = 0.0f;
};
}

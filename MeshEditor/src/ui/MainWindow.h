// File: MeshEditor/src/ui/MainWindow.h
#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>

namespace MeshEditor
{
class Dock_Outliner;
class Dock_Properties;
class Dock_Content;
class Dock_Console;
class ViewportWidget;
class Scene;
class ToolManager;
class UndoStack;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void OnImportMesh();
    void OnExportSelected();
    void OnUndo();
    void OnRedo();
    void OnResetLayout();
    void OnAbout();

private:
    void BuildMenu();
    void BuildToolbar();
    void BuildStatus();
    void BuildDocks();
    void UpdateStatus();

    Dock_Outliner* m_outliner;
    Dock_Properties* m_properties;
    Dock_Content* m_content;
    Dock_Console* m_console;
    ViewportWidget* m_viewport;

    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<ToolManager> m_tools;
    std::unique_ptr<UndoStack> m_undo;
    QTimer m_statusTimer;
};
}

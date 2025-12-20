// File: MeshEditor/src/ui/MainWindow.cpp
#include "MainWindow.h"
#include "Dock_Outliner.h"
#include "Dock_Properties.h"
#include "Dock_Content.h"
#include "Dock_Console.h"
#include "ViewportWidget.h"
#include "engine/UndoStack.h"
#include "scene/Scene.h"
#include "tools/ToolManager.h"
#include "assets/Importer.h"
#include "assets/Export_OBJ.h"
#include "engine/Logger.h"

#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>

namespace MeshEditor
{
MainWindow::MainWindow()
    : m_outliner(nullptr)
    , m_properties(nullptr)
    , m_content(nullptr)
    , m_console(nullptr)
    , m_viewport(nullptr)
{
    setWindowTitle("MeshEditor");
    resize(1400, 900);

    m_scene = std::make_unique<Scene>();
    m_tools = std::make_unique<ToolManager>();
    m_undo = std::make_unique<UndoStack>();

    BuildMenu();
    BuildToolbar();
    BuildStatus();
    BuildDocks();

    m_viewport->SetScene(m_scene.get());
    m_viewport->SetToolManager(m_tools.get());
    m_viewport->SetUndoStack(m_undo.get());

    m_outliner->BindScene(m_scene.get());
    m_properties->BindScene(m_scene.get());
    m_properties->BindUndoStack(m_undo.get());
    m_content->BindScene(m_scene.get());
    m_console->BindLogger();

    connect(m_outliner, &Dock_Outliner::EntitySelected, m_viewport, &ViewportWidget::OnEntitySelected);
    connect(m_viewport, &ViewportWidget::SelectionChanged, m_outliner, &Dock_Outliner::OnSelectionChanged);
    connect(m_viewport, &ViewportWidget::SelectionChanged, m_properties, &Dock_Properties::OnSelectionChanged);

    m_statusTimer.setInterval(250);
    connect(&m_statusTimer, &QTimer::timeout, this, &MainWindow::UpdateStatus);
    m_statusTimer.start();

    statusBar()->showMessage("Ready");
}

void MainWindow::BuildMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    QAction* actionNew = fileMenu->addAction("New");
    QAction* actionOpen = fileMenu->addAction("Open...");
    QAction* actionSave = fileMenu->addAction("Save...");
    QAction* actionImport = fileMenu->addAction("Import Mesh...");
    QAction* actionExport = fileMenu->addAction("Export Selected...");
    fileMenu->addSeparator();
    QAction* actionExit = fileMenu->addAction("Exit");

    connect(actionImport, &QAction::triggered, this, &MainWindow::OnImportMesh);
    connect(actionExport, &QAction::triggered, this, &MainWindow::OnExportSelected);
    connect(actionExit, &QAction::triggered, this, &MainWindow::close);
    connect(actionNew, &QAction::triggered, [](){ Logger::Info("New project stub."); });
    connect(actionOpen, &QAction::triggered, [](){ Logger::Info("Open project stub."); });
    connect(actionSave, &QAction::triggered, [](){ Logger::Info("Save project stub."); });

    QMenu* editMenu = menuBar()->addMenu("&Edit");
    QAction* actionUndo = editMenu->addAction("Undo");
    QAction* actionRedo = editMenu->addAction("Redo");
    editMenu->addSeparator();
    QAction* actionPrefs = editMenu->addAction("Preferences");

    connect(actionUndo, &QAction::triggered, this, &MainWindow::OnUndo);
    connect(actionRedo, &QAction::triggered, this, &MainWindow::OnRedo);
    connect(actionPrefs, &QAction::triggered, [](){ Logger::Info("Preferences stub."); });

    QMenu* viewMenu = menuBar()->addMenu("&View");
    QAction* actionReset = viewMenu->addAction("Reset Layout");
    connect(actionReset, &QAction::triggered, this, &MainWindow::OnResetLayout);

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    QAction* actionAbout = helpMenu->addAction("About");
    connect(actionAbout, &QAction::triggered, this, &MainWindow::OnAbout);
}

void MainWindow::BuildToolbar()
{
    QToolBar* toolbar = addToolBar("Tools");
    QAction* selectTool = toolbar->addAction("Select");
    QAction* moveTool = toolbar->addAction("Move");
    QAction* rotateTool = toolbar->addAction("Rotate");
    QAction* scaleTool = toolbar->addAction("Scale");
    toolbar->addSeparator();
    QAction* wireframe = toolbar->addAction("Wireframe");
    QAction* shaded = toolbar->addAction("Shaded");

    connect(selectTool, &QAction::triggered, [this]() { m_tools->SetActiveTool("Select"); });
    connect(moveTool, &QAction::triggered, [this]() { m_tools->SetActiveTool("Move"); });
    connect(rotateTool, &QAction::triggered, [this]() { m_tools->SetActiveTool("Rotate"); });
    connect(scaleTool, &QAction::triggered, [this]() { m_tools->SetActiveTool("Scale"); });
    connect(wireframe, &QAction::triggered, [this]() { m_viewport->ToggleWireframe(); });
    connect(shaded, &QAction::triggered, [this]() { m_viewport->ToggleShaded(); });
}

void MainWindow::BuildStatus()
{
    statusBar()->addPermanentWidget(new QLabel("FPS: --"), 1);
    statusBar()->addPermanentWidget(new QLabel("Selection: 0"), 1);
    statusBar()->addPermanentWidget(new QLabel("Tool: Select"), 1);
}

void MainWindow::BuildDocks()
{
    m_outliner = new Dock_Outliner(this);
    m_properties = new Dock_Properties(this);
    m_content = new Dock_Content(this);
    m_console = new Dock_Console(this);
    m_viewport = new ViewportWidget(this);

    setCentralWidget(m_viewport);

    addDockWidget(Qt::LeftDockWidgetArea, m_outliner);
    addDockWidget(Qt::RightDockWidgetArea, m_properties);
    addDockWidget(Qt::BottomDockWidgetArea, m_console);
    addDockWidget(Qt::BottomDockWidgetArea, m_content);

    tabifyDockWidget(m_console, m_content);
    m_console->raise();
}

void MainWindow::UpdateStatus()
{
    auto labels = statusBar()->findChildren<QLabel*>();
    if (labels.size() >= 3)
    {
        labels[0]->setText(QString("FPS: %1").arg(m_viewport->GetFPS(), 0, 'f', 1));
        labels[1]->setText(QString("Selection: %1").arg(m_scene->GetSelection().Count()));
        labels[2]->setText(QString("Tool: %1").arg(QString::fromStdString(m_tools->GetActiveToolName())));
    }
}

void MainWindow::OnImportMesh()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Import Mesh", QString(), "Mesh Files (*.obj *.gltf *.glb)");
    if (filePath.isEmpty())
        return;

    Importer importer;
    importer.Import(filePath.toStdString(), *m_scene, *m_undo);
    m_outliner->Refresh();
    m_content->Refresh();
}

void MainWindow::OnExportSelected()
{
    if (!m_scene->GetSelection().HasSelection())
    {
        QMessageBox::information(this, "Export", "No selection to export.");
        return;
    }
    QString filePath = QFileDialog::getSaveFileName(this, "Export OBJ", QString(), "OBJ Files (*.obj)");
    if (filePath.isEmpty())
        return;

    Export_OBJ exporter;
    exporter.ExportSelected(filePath.toStdString(), *m_scene);
}

void MainWindow::OnUndo()
{
    m_undo->Undo();
    m_outliner->Refresh();
    m_properties->Refresh();
}

void MainWindow::OnRedo()
{
    m_undo->Redo();
    m_outliner->Refresh();
    m_properties->Refresh();
}

void MainWindow::OnResetLayout()
{
    removeDockWidget(m_outliner);
    removeDockWidget(m_properties);
    removeDockWidget(m_console);
    removeDockWidget(m_content);
    BuildDocks();
}

void MainWindow::OnAbout()
{
    QMessageBox::about(this, "About MeshEditor", "MeshEditor - Blender-inspired mesh editor prototype.");
}
}

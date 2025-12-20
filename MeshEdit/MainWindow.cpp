#include "MainWindow.h"

#include <QAction>
#include <QDockWidget>
#include <QFileDialog>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>
#include "ViewportWidget.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    setWindowTitle("Broken Arrow MeshEdit");
    viewport_ = new ViewportWidget(this);
    setCentralWidget(viewport_);
    createDockPanels();
    createMenus();
    statusBar()->showMessage("Ready");
}

void MainWindow::createDockPanels() {
    auto* outlinerDock = new QDockWidget("Outliner", this);
    outlinerDock->setObjectName("OutlinerDock");
    outlinerDock->setWidget(new QListWidget(outlinerDock));
    addDockWidget(Qt::LeftDockWidgetArea, outlinerDock);

    auto* sceneDock = new QDockWidget("Scene", this);
    sceneDock->setObjectName("SceneDock");
    sceneDock->setWidget(new QTextEdit(sceneDock));
    addDockWidget(Qt::LeftDockWidgetArea, sceneDock);

    auto* propertiesDock = new QDockWidget("Properties", this);
    propertiesDock->setObjectName("PropertiesDock");
    propertiesDock->setWidget(new QTextEdit(propertiesDock));
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);

    auto* toolDock = new QDockWidget("Tool Shelf", this);
    toolDock->setObjectName("ToolDock");
    toolDock->setWidget(new QLabel("Viewport tools go here.", toolDock));
    addDockWidget(Qt::RightDockWidgetArea, toolDock);
}

void MainWindow::createMenus() {
    auto* fileMenu = menuBar()->addMenu("&File");

    auto* openAction = new QAction("Open Mesh...", this);
    connect(openAction, &QAction::triggered, this, [this]() {
        const QString filter = "Mesh Files (*.obj *.gltf *.glb)";
        const QString fileName = QFileDialog::getOpenFileName(this, "Open Mesh", QString(), filter);
        if (!fileName.isEmpty()) {
            statusBar()->showMessage(QString("Loaded %1").arg(fileName), 4000);
        }
    });
    fileMenu->addAction(openAction);

    auto* importAction = new QAction("Import Texture...", this);
    connect(importAction, &QAction::triggered, this, [this]() {
        const QString filter = "Images (*.png *.jpg *.jpeg)";
        const QString fileName = QFileDialog::getOpenFileName(this, "Import Texture", QString(), filter);
        if (!fileName.isEmpty()) {
            statusBar()->showMessage(QString("Imported %1").arg(fileName), 4000);
        }
    });
    fileMenu->addAction(importAction);

    fileMenu->addSeparator();

    auto* exitAction = new QAction("Exit", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    auto* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Reset Layout", this, [this]() {
        restoreState(QByteArray());
    });

    auto* toolbar = addToolBar("Main");
    toolbar->addAction(openAction);
    toolbar->addAction(importAction);
}

// File: MeshEditor/src/ui/MainWindow.cpp
#include "ui/MainWindow.h"

#include "assets/Export_OBJ.h"
#include "assets/Importer.h"
#include "engine/Logger.h"
#include "engine/Time.h"

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QTimer>
#include <algorithm>

namespace MeshEditor
{
    MainWindow::MainWindow(App& app, QWidget* parent)
        : QMainWindow(parent), mApp(app)
    {
        mViewport = new ViewportWidget(app, this);
        setCentralWidget(mViewport);

        mOutliner = new Dock_Outliner(app, this);
        mProperties = new Dock_Properties(app, this);
        mContent = new Dock_Content(app, this);
        mConsole = new Dock_Console(this);

        addDockWidget(Qt::LeftDockWidgetArea, mOutliner);
        addDockWidget(Qt::RightDockWidgetArea, mProperties);
        addDockWidget(Qt::BottomDockWidgetArea, mConsole);
        addDockWidget(Qt::BottomDockWidgetArea, mContent);

        BuildMenus();
        BuildToolbar();

        mStatusFps = new QLabel("FPS: --", this);
        mStatusSelection = new QLabel("Selection: 0", this);
        mStatusTool = new QLabel("Tool: Select", this);
        statusBar()->addPermanentWidget(mStatusFps);
        statusBar()->addPermanentWidget(mStatusSelection);
        statusBar()->addPermanentWidget(mStatusTool);

        Logger::SetCallback([this](const std::string& message)
        {
            QMetaObject::invokeMethod(mConsole, [this, message]()
            {
                mConsole->AppendLine(QString::fromStdString(message));
            });
        });

        mApp.GetToolManager().Activate("Select", mViewport);
        mOutliner->Refresh();
        mContent->Refresh();

        auto* statusTimer = new QTimer(this);
        connect(statusTimer, &QTimer::timeout, this, [this]() { UpdateStatus(); });
        statusTimer->start(500);
    }

    void MainWindow::BuildMenus()
    {
        QMenu* fileMenu = menuBar()->addMenu("&File");
        QAction* newAction = fileMenu->addAction("New");
        QAction* openAction = fileMenu->addAction("Open...");
        QAction* saveAction = fileMenu->addAction("Save...");
        fileMenu->addSeparator();
        QAction* importAction = fileMenu->addAction("Import Mesh...");
        QAction* exportAction = fileMenu->addAction("Export Selected...");
        fileMenu->addSeparator();
        QAction* exitAction = fileMenu->addAction("Exit");

        connect(newAction, &QAction::triggered, this, []()
        {
            QMessageBox::information(nullptr, "New", "New project is a stub.");
        });
        connect(openAction, &QAction::triggered, this, []()
        {
            QMessageBox::information(nullptr, "Open", "Open project is a stub.");
        });
        connect(saveAction, &QAction::triggered, this, []()
        {
            QMessageBox::information(nullptr, "Save", "Save project is a stub.");
        });
        connect(importAction, &QAction::triggered, this, [this]() { ImportMesh(); });
        connect(exportAction, &QAction::triggered, this, [this]() { ExportSelected(); });
        connect(exitAction, &QAction::triggered, this, [this]() { close(); });

        QMenu* editMenu = menuBar()->addMenu("&Edit");
        QAction* undoAction = editMenu->addAction("Undo");
        QAction* redoAction = editMenu->addAction("Redo");
        QAction* prefsAction = editMenu->addAction("Preferences...");
        connect(undoAction, &QAction::triggered, this, [this]() { mApp.GetUndoStack().Undo(); });
        connect(redoAction, &QAction::triggered, this, [this]() { mApp.GetUndoStack().Redo(); });
        connect(prefsAction, &QAction::triggered, this, []()
        {
            QMessageBox::information(nullptr, "Preferences", "Preferences are a stub.");
        });

        QMenu* viewMenu = menuBar()->addMenu("&View");
        QAction* resetAction = viewMenu->addAction("Reset Layout");
        connect(resetAction, &QAction::triggered, this, [this]() { ResetLayout(); });

        QMenu* helpMenu = menuBar()->addMenu("&Help");
        QAction* aboutAction = helpMenu->addAction("About");
        connect(aboutAction, &QAction::triggered, this, []()
        {
            QMessageBox::about(nullptr, "About MeshEditor", "MeshEditor - Blender-inspired mesh editor prototype.");
        });
    }

    void MainWindow::BuildToolbar()
    {
        QToolBar* toolbar = addToolBar("Tools");
        QAction* selectAction = toolbar->addAction("Select");
        QAction* moveAction = toolbar->addAction("Move");
        QAction* rotateAction = toolbar->addAction("Rotate");
        QAction* scaleAction = toolbar->addAction("Scale");
        toolbar->addSeparator();
        QAction* wireframeAction = toolbar->addAction("Wireframe");
        QAction* shadedAction = toolbar->addAction("Shaded");
        wireframeAction->setCheckable(true);
        shadedAction->setCheckable(true);
        shadedAction->setChecked(true);

        connect(selectAction, &QAction::triggered, this, [this]()
        {
            mApp.GetToolManager().Activate("Select", mViewport);
            mStatusTool->setText("Tool: Select");
        });
        connect(moveAction, &QAction::triggered, this, [this]()
        {
            mApp.GetToolManager().Activate("Move", mViewport);
            mStatusTool->setText("Tool: Move");
        });
        connect(rotateAction, &QAction::triggered, this, [this]()
        {
            mApp.GetToolManager().Activate("Rotate", mViewport);
            mStatusTool->setText("Tool: Rotate");
        });
        connect(scaleAction, &QAction::triggered, this, [this]()
        {
            mApp.GetToolManager().Activate("Scale", mViewport);
            mStatusTool->setText("Tool: Scale");
        });

        connect(wireframeAction, &QAction::triggered, this, []()
        {
            Logger::Info("Wireframe toggle is a stub.");
        });
        connect(shadedAction, &QAction::triggered, this, []()
        {
            Logger::Info("Shaded toggle is a stub.");
        });
    }

    void MainWindow::UpdateStatus()
    {
        float fps = 1.0f / std::max(0.0001f, Time::DeltaSeconds());
        mStatusFps->setText(QString("FPS: %1").arg(fps, 0, 'f', 1));
        mStatusSelection->setText(QString("Selection: %1").arg(mApp.GetSelection().HasSelection() ? 1 : 0));
        auto* tool = mApp.GetToolManager().ActiveTool();
        if (tool)
        {
            mStatusTool->setText(QString("Tool: %1").arg(tool->Name()));
        }
        mOutliner->Refresh();
        mProperties->Refresh();
        mContent->Refresh();
    }

    void MainWindow::ImportMesh()
    {
        QString file = QFileDialog::getOpenFileName(this, "Import Mesh", QString(), "Mesh Files (*.obj *.gltf *.glb)");
        if (file.isEmpty())
        {
            return;
        }
        std::string error;
        Entity* entity = nullptr;
        if (Importer::ImportMesh(file.toStdString(), mApp.GetScene(), entity, error))
        {
            mApp.GetAssetDatabase().AddAsset(file.toStdString());
            if (entity)
            {
                uint64_t id = entity->Id();
                mApp.GetUndoStack().Push(Command("Import Mesh", [this, id, file]()
                {
                    Q_UNUSED(file);
                    // already imported
                }, [this, id]()
                {
                    mApp.GetScene().RemoveEntity(id);
                }));
            }
            mOutliner->Refresh();
        }
        else
        {
            QMessageBox::warning(this, "Import Failed", QString::fromStdString(error));
        }
    }

    void MainWindow::ExportSelected()
    {
        if (!mApp.GetSelection().HasSelection())
        {
            QMessageBox::information(this, "Export", "No selection.");
            return;
        }
        auto* entity = mApp.GetScene().FindEntity(mApp.GetSelection().Selected());
        if (!entity)
        {
            return;
        }
        QString file = QFileDialog::getSaveFileName(this, "Export OBJ", QString(), "OBJ (*.obj)");
        if (file.isEmpty())
        {
            return;
        }
        std::string error;
        if (!ExportOBJ(file.toStdString(), *entity, error))
        {
            QMessageBox::warning(this, "Export Failed", QString::fromStdString(error));
        }
    }

    void MainWindow::ResetLayout()
    {
        addDockWidget(Qt::LeftDockWidgetArea, mOutliner);
        addDockWidget(Qt::RightDockWidgetArea, mProperties);
        addDockWidget(Qt::BottomDockWidgetArea, mConsole);
        addDockWidget(Qt::BottomDockWidgetArea, mContent);
    }
}

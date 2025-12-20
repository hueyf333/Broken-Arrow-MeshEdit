// File: MeshEditor/src/ui/MainWindow.cpp
#include "MainWindow.h"

#include <QAction>
#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeWidget>

#include "engine/Logger.h"
#include "engine/Command.h"
#include "assets/Importer.h"
#include "assets/Export_OBJ.h"
#include "render/RenderMesh.h"
#include "ui/Dock_Outliner.h"
#include "ui/Dock_Properties.h"
#include "ui/Dock_Content.h"
#include "ui/Dock_Console.h"
#include "ui/ViewportWidget.h"
#include "tools/Tool_Select.h"
#include "tools/Tool_Transform.h"

#include <glm/glm.hpp>

namespace mesh
{
namespace
{
class ImportCommand final : public Command
{
public:
    ImportCommand(Scene& scene, Selection& selection, MeshData meshData, const std::string& name)
        : scene_(scene), selection_(selection), meshData_(std::move(meshData)), name_(name)
    {
    }

    void Execute() override
    {
        Entity& entity = scene_.CreateEntity(name_);
        entity.Mesh().mesh = std::make_shared<RenderMesh>();
        if (rendererDevice_)
        {
            entity.Mesh().mesh->Initialize(rendererDevice_, meshData_);
        }
        entity.Material().baseColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        createdId_ = entity.Id();
        selection_.Set(entity.Id());
    }

    void Undo() override
    {
        if (createdId_ != 0)
        {
            scene_.DestroyEntity(createdId_);
            selection_.Clear();
        }
    }

    std::string Name() const override
    {
        return "Import Mesh";
    }

    void SetDevice(ID3D11Device* device)
    {
        rendererDevice_ = device;
    }

private:
    Scene& scene_;
    Selection& selection_;
    MeshData meshData_;
    std::string name_;
    EntityId createdId_ = 0;
    ID3D11Device* rendererDevice_ = nullptr;
};

}

MainWindow::MainWindow()
{
    setWindowTitle("MeshEditor");
    resize(1400, 900);

    viewport_ = new ViewportWidget(scene_, selection_, toolManager_, this);
    setCentralWidget(viewport_);

    CreateMenus();
    CreateToolbar();
    CreateDocks();

    statusBar();

    toolManager_.Register(std::make_unique<Tool_Select>(scene_, selection_, *viewport_));
    toolManager_.Register(std::make_unique<Tool_Transform>(scene_, selection_, *viewport_, undoStack_, ToolType::Move));
    toolManager_.Register(std::make_unique<Tool_Transform>(scene_, selection_, *viewport_, undoStack_, ToolType::Rotate));
    toolManager_.Register(std::make_unique<Tool_Transform>(scene_, selection_, *viewport_, undoStack_, ToolType::Scale));
    toolManager_.SetActive(ToolType::Select);

    connect(outlinerDock_->Tree(), &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem*, int) { OnOutlinerSelectionChanged(); });
    propertiesDock_->SetOnEdited([this]() { OnPropertiesChanged(); });
    viewport_->SetFpsCallback([this](float fps) { OnViewportFps(fps); });
    viewport_->SetSelectionCallback([this]() { OnViewportSelectionChanged(); });

    PopulateDefaultScene();
    UpdateOutliner();
    UpdateStatus();
}

void MainWindow::CreateMenus()
{
    auto fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("New");
    fileMenu->addAction("Open");
    fileMenu->addAction("Save");
    fileMenu->addSeparator();
    fileMenu->addAction("Import Mesh...", this, &MainWindow::OnImportMesh);
    fileMenu->addAction("Export Selected...", this, &MainWindow::OnExportSelected);
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close);

    auto editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction("Undo", this, &MainWindow::OnUndo, QKeySequence::Undo);
    editMenu->addAction("Redo", this, &MainWindow::OnRedo, QKeySequence::Redo);
    editMenu->addAction("Preferences");

    auto viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("Reset Layout", this, &MainWindow::OnResetLayout);

    auto helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("About", []()
    {
        QMessageBox::about(nullptr, "About MeshEditor", "MeshEditor - Blender-inspired mesh editor prototype.");
    });
}

void MainWindow::CreateToolbar()
{
    auto toolbar = addToolBar("Tools");
    toolbar->setMovable(false);

    auto selectAction = toolbar->addAction("Select", this, &MainWindow::OnSelectTool);
    auto moveAction = toolbar->addAction("Move", this, &MainWindow::OnMoveTool);
    auto rotateAction = toolbar->addAction("Rotate", this, &MainWindow::OnRotateTool);
    auto scaleAction = toolbar->addAction("Scale", this, &MainWindow::OnScaleTool);

    toolbar->addSeparator();

    auto wireframeAction = toolbar->addAction("Wireframe");
    wireframeAction->setCheckable(true);
    connect(wireframeAction, &QAction::toggled, this, &MainWindow::OnToggleWireframe);

    auto shadedAction = toolbar->addAction("Shaded");
    shadedAction->setCheckable(true);
    shadedAction->setChecked(true);
    connect(shadedAction, &QAction::toggled, this, &MainWindow::OnToggleShaded);

    selectAction->setCheckable(true);
    moveAction->setCheckable(true);
    rotateAction->setCheckable(true);
    scaleAction->setCheckable(true);

    QActionGroup* group = new QActionGroup(toolbar);
    group->addAction(selectAction);
    group->addAction(moveAction);
    group->addAction(rotateAction);
    group->addAction(scaleAction);
    selectAction->setChecked(true);
}

void MainWindow::CreateDocks()
{
    outlinerDock_ = new Dock_Outliner(this);
    addDockWidget(Qt::LeftDockWidgetArea, outlinerDock_);

    propertiesDock_ = new Dock_Properties(this);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock_);

    contentDock_ = new Dock_Content(this);
    addDockWidget(Qt::BottomDockWidgetArea, contentDock_);

    consoleDock_ = new Dock_Console(this);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock_);

    tabifyDockWidget(contentDock_, consoleDock_);
    contentDock_->raise();

    Logger::Get().Attach(consoleDock_->Console());
}

void MainWindow::PopulateDefaultScene()
{
    viewport_->EnsureRenderer();
    Entity& entity = scene_.CreateEntity("Cube");
    MeshData meshData;
    meshData.vertices = {
        {{-1, -1, -1}, {0, 0, -1}, {0, 1}},
        {{1, -1, -1}, {0, 0, -1}, {1, 1}},
        {{1, 1, -1}, {0, 0, -1}, {1, 0}},
        {{-1, 1, -1}, {0, 0, -1}, {0, 0}},
        {{-1, -1, 1}, {0, 0, 1}, {0, 1}},
        {{1, -1, 1}, {0, 0, 1}, {1, 1}},
        {{1, 1, 1}, {0, 0, 1}, {1, 0}},
        {{-1, 1, 1}, {0, 0, 1}, {0, 0}},
    };
    meshData.indices = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        0, 4, 5, 0, 5, 1,
        1, 5, 6, 1, 6, 2,
        2, 6, 7, 2, 7, 3,
        3, 7, 4, 3, 4, 0
    };
    meshData.boundsMin = glm::vec3(-1.0f);
    meshData.boundsMax = glm::vec3(1.0f);

    entity.Mesh().mesh = std::make_shared<RenderMesh>();
    if (viewport_->GetDevice())
    {
        entity.Mesh().mesh->Initialize(viewport_->GetDevice(), meshData);
    }
    selection_.Set(entity.Id());
}

void MainWindow::OnImportMesh()
{
    const QString path = QFileDialog::getOpenFileName(this, "Import Mesh", QString(), "Mesh Files (*.obj *.gltf *.glb)");
    if (path.isEmpty())
    {
        return;
    }

    Importer importer;
    MeshData meshData;
    if (!importer.Import(path.toStdString(), meshData))
    {
        Logger::Get().Error("Failed to import mesh.");
        return;
    }

    auto command = std::make_unique<ImportCommand>(scene_, selection_, meshData, path.toStdString());
    command->SetDevice(viewport_->GetDevice());
    undoStack_.Execute(std::move(command));
    UpdateOutliner();
    UpdateStatus();
}

void MainWindow::OnExportSelected()
{
    if (!selection_.HasSelection())
    {
        return;
    }

    const QString path = QFileDialog::getSaveFileName(this, "Export OBJ", QString(), "OBJ Files (*.obj)");
    if (path.isEmpty())
    {
        return;
    }

    Entity* entity = scene_.Find(selection_.Selected().value());
    if (!entity || !entity->Mesh().mesh)
    {
        return;
    }

    ExportOBJ exporter;
    exporter.Export(path.toStdString(), entity->Mesh().mesh->Data());
}

void MainWindow::OnUndo()
{
    undoStack_.Undo();
    UpdateOutliner();
    UpdateStatus();
}

void MainWindow::OnRedo()
{
    undoStack_.Redo();
    UpdateOutliner();
    UpdateStatus();
}

void MainWindow::OnResetLayout()
{
    addDockWidget(Qt::LeftDockWidgetArea, outlinerDock_);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock_);
    addDockWidget(Qt::BottomDockWidgetArea, contentDock_);
    addDockWidget(Qt::BottomDockWidgetArea, consoleDock_);
    tabifyDockWidget(contentDock_, consoleDock_);
    contentDock_->raise();
}

void MainWindow::OnSelectTool()
{
    toolManager_.SetActive(ToolType::Select);
    UpdateStatus();
}

void MainWindow::OnMoveTool()
{
    toolManager_.SetActive(ToolType::Move);
    UpdateStatus();
}

void MainWindow::OnRotateTool()
{
    toolManager_.SetActive(ToolType::Rotate);
    UpdateStatus();
}

void MainWindow::OnScaleTool()
{
    toolManager_.SetActive(ToolType::Scale);
    UpdateStatus();
}

void MainWindow::OnToggleWireframe(bool checked)
{
    viewport_->SetWireframe(checked);
}

void MainWindow::OnToggleShaded(bool checked)
{
    viewport_->SetShaded(checked);
}

void MainWindow::OnOutlinerSelectionChanged()
{
    auto id = outlinerDock_->SelectedEntity();
    if (id)
    {
        selection_.Set(*id);
    }
    else
    {
        selection_.Clear();
    }
    propertiesDock_->SetSelection(scene_, selection_);
    outlinerDock_->Rebuild(scene_, selection_);
    UpdateStatus();
}

void MainWindow::OnViewportSelectionChanged()
{
    propertiesDock_->SetSelection(scene_, selection_);
    outlinerDock_->Rebuild(scene_, selection_);
    UpdateStatus();
}

void MainWindow::OnPropertiesChanged()
{
    if (!selection_.HasSelection())
    {
        return;
    }
    auto* entity = scene_.Find(selection_.Selected().value());
    if (!entity)
    {
        return;
    }
    TransformComponent before = entity->Transform();
    propertiesDock_->ApplyToEntity(*entity);
    TransformComponent after = entity->Transform();
    undoStack_.Execute(std::make_unique<FunctionCommand>(
        "Transform",
        [entity, after]() { entity->Transform() = after; },
        [entity, before]() { entity->Transform() = before; }));
    UpdateStatus();
}

void MainWindow::OnViewportFps(float fps)
{
    lastFps_ = fps;
    UpdateStatus();
}

void MainWindow::UpdateOutliner()
{
    outlinerDock_->Rebuild(scene_, selection_);
    propertiesDock_->SetSelection(scene_, selection_);
}

void MainWindow::UpdateStatus()
{
    QString toolName = toolManager_.ActiveToolName();
    int selectionCount = selection_.HasSelection() ? 1 : 0;
    statusBar()->showMessage(QString("FPS: %1 | Selected: %2 | Tool: %3").arg(lastFps_, 0, 'f', 1).arg(selectionCount).arg(toolName));
}
}

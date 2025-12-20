// File: MeshEditor/src/app/App.cpp
#include "App.h"

#include <QSurfaceFormat>
#include "ui/MainWindow.h"

namespace mesh
{
App::App() = default;

void App::Initialize()
{
    mainWindow_ = std::make_unique<MainWindow>();
    mainWindow_->show();
}
}

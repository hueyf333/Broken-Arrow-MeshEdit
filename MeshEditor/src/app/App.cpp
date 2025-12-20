// File: MeshEditor/src/app/App.cpp
#include "App.h"
#include "ui/MainWindow.h"
#include <objbase.h>

namespace MeshEditor
{
App::App(int& argc, char** argv)
    : m_app(argc, argv)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    m_mainWindow = std::make_unique<MainWindow>();
    m_mainWindow->show();
}

int App::Run()
{
    int result = m_app.exec();
    CoUninitialize();
    return result;
}
}

// File: MeshEditor/src/app/App.h
#pragma once

#include <QApplication>
#include <memory>

namespace MeshEditor
{
class MainWindow;

class App
{
public:
    App(int& argc, char** argv);
    int Run();

private:
    QApplication m_app;
    std::unique_ptr<MainWindow> m_mainWindow;
};
}

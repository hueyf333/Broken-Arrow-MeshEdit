// File: MeshEditor/src/app/App.h
#pragma once

#include <memory>

namespace mesh
{
class MainWindow;

class App
{
public:
    App();
    void Initialize();

private:
    std::unique_ptr<MainWindow> mainWindow_;
};
}

// File: MeshEditor/src/app/Main.cpp
#include "app/App.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <windows.h>

int main(int argc, char** argv)
{
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    QApplication app(argc, argv);
    MeshEditor::App editorApp;
    MeshEditor::MainWindow window(editorApp);
    window.resize(1400, 900);
    window.show();
    int result = app.exec();
    CoUninitialize();
    return result;
}

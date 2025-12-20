// File: MeshEditor/src/app/Main.cpp
#include <QApplication>
#include "App.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    mesh::App editorApp;
    editorApp.Initialize();
    return app.exec();
}

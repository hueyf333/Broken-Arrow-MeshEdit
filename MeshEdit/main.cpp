#include <QApplication>
#include <objbase.h>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    QApplication app(argc, argv);
    MainWindow window;
    window.resize(1400, 900);
    window.show();
    const int result = app.exec();
    CoUninitialize();
    return result;
}

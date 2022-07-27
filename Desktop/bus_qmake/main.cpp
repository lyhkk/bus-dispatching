#include "mainwindow.h"

#include <QApplication>

bus *bus::self = nullptr;
MainWindow *MainWindow::self = nullptr;
std::mutex m;
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

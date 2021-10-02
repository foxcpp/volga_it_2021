#include <QApplication>
#include "mainwindow.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MainWindow* wnd = new MainWindow();
    wnd->show();

    return app.exec();
}

#include "mainwindow.h"
#include <QApplication>
#include <QThreadPool>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QThreadPool::globalInstance()->setMaxThreadCount(4);

    MainWindow w;
    w.show();

    return a.exec();
}

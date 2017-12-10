#include "mainwindow.h"
#include <QApplication>
#include <QThreadPool>
#include <QDir>

#include <package_system/package_manager.h>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QThreadPool::globalInstance()->setMaxThreadCount(4);

    QStringList package_paths;
    QDir rootDir = QApplication::applicationDirPath();
    rootDir.cdUp();
    const QString rootDirPath = rootDir.canonicalPath();
    QString plugin_path = rootDirPath + "/packages";
    package_paths.append(plugin_path);

    QPointer<PackageSystem::PackageManager> package_manager = new PackageSystem::PackageManager;
    package_manager->setPackagePaths(package_paths);
    package_manager->loadPackages();

    Core::MainWindow w{package_manager};
    w.show();

    return a.exec();
}

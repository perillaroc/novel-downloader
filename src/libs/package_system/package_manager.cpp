#include "package_manager.h"

#include <QDir>
#include <QFileInfo>
#include <QtDebug>

#include "package_spec.h"

using namespace PackageSystem;


PackageManager::PackageManager(QObject *parent):
    QObject{parent}
{
}

PackageManager::~PackageManager()
{

}

void PackageManager::setPackagePaths(const QStringList &package_paths)
{
    package_paths_ = package_paths;
    readPackagePaths();
}

void PackageManager::loadPackages()
{
    QList<QSharedPointer<PackageSpec>> load_queue = package_specs_;

    QString error_string;
    foreach(QSharedPointer<PackageSpec> package_spec, load_queue)
    {
        package_spec->loadLibrary();
    }

    foreach(QSharedPointer<PackageSpec> package_spec, load_queue)
    {
        package_spec->initializePlugin();
    }

    QListIterator<QSharedPointer<PackageSpec>> iter(load_queue);
    iter.toBack();
    while(iter.hasPrevious())
    {
        QSharedPointer<PackageSpec> package_spec = iter.previous();
        package_spec->pluginsInitialized();
    }
}

void PackageManager::addObject(QObject *obj)
{
    all_objects_.append(obj);
}

void PackageManager::removeObject(QObject *obj)
{
    all_objects_.removeAll(obj);
}

QList<QObject *> PackageManager::allObjects()
{
    return all_objects_;
}

void PackageManager::readPackagePaths()
{
    QStringList package_json_files;

    QStringList search_paths = package_paths_;
    while (!search_paths.isEmpty())
    {
        const QDir dir(search_paths.takeFirst());
        const QFileInfoList dirs = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        foreach (const QFileInfo &subdir, dirs)
        {
            qDebug()<<subdir.absoluteFilePath();
            QDir sub_dir{subdir.absoluteFilePath()};
            const QFileInfoList files = sub_dir.entryInfoList(QDir::Files | QDir::NoSymLinks);
            foreach (const QFileInfo &file, files)
            {
                if (file.fileName() == "package.json")
                {
                    const QString file_path = file.absoluteFilePath();
                    package_json_files.append(file_path);
                }
            }
        }
    }


    foreach(QString package_file_path, package_json_files)
    {
        QSharedPointer<PackageSpec> spec{new PackageSpec{this}};
        if(!spec->read(package_file_path))
        {
            qDebug()<<"[PackageManager::loadPackages] can't read package spec"<<package_file_path;
            spec.clear();
            continue;
        }
        package_specs_.push_back(spec);
    }
}

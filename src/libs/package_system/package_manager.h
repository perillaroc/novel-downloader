#pragma once
#include <QObject>
#include <QList>
#include <QPointer>

#include "package_system_global.h"

namespace PackageSystem{

class PackageSpec;

class PACKAGE_SYSTEM_EXPORT PackageManager: public QObject
{
    Q_OBJECT
public:
    PackageManager(QObject *parent=nullptr);
    ~PackageManager();

    void setPackagePaths(const QStringList &package_paths);
    void loadPackages();

    void addObject(QObject *obj);
    void removeObject(QObject *obj);
    QList<QObject *> allObjects();

    template <typename T> QList<T*> getObjects()
    {
        QList<T*> results;
        QList<QObject *> all = allObjects();
        foreach (QObject *obj, all) {
            T* result = qobject_cast<T*>(obj);
            if (result)
                results += result;
        }
        return results;
    }

private:
    void readPackagePaths();

    QList<QSharedPointer<PackageSpec>> package_specs_;

    QStringList package_paths_;

    QList<QObject *> all_objects_;
};

}

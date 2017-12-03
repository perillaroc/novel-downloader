#pragma once
#include <QObject>
#include <QList>

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

private:
    void readPackagePaths();

    QList<QSharedPointer<PackageSpec>> package_specs_;

    QStringList package_paths_;
};

}

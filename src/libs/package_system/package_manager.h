#pragma once
#include <QObject>

#include "package_system_global.h"

namespace PackageSystem{

class PACKAGE_SYSTEM_EXPORT PackageManager: public QObject
{
    Q_OBJECT
public:
    PackageManager(QObject *parent=nullptr);
    ~PackageManager();

    void loadPackages();

private:

};

}

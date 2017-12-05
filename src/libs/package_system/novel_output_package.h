#pragma once

#include "package_interface.h"

#include <QDir>
#include <QJsonObject>

namespace PackageSystem{

class PluginManager;

class PACKAGE_SYSTEM_EXPORT NovelOutputPackage : public PackageInterface
{
    Q_OBJECT
public:
    explicit NovelOutputPackage(QPointer<PackageManager> package_manager, QObject *parent = nullptr);

    virtual bool initialize(const QStringList& arguments, QString* error_string);
    virtual void pluginsInitialized();
    virtual void aboutToShutDown();

signals:

public slots:

private:

};

}

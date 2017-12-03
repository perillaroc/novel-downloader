#pragma once

#include "package_interface.h"

namespace PackageSystem{

class PluginManager;

class NovelWebsitePackage : public PackageInterface
{
    Q_OBJECT
public:
    explicit NovelWebsitePackage(QObject *parent = nullptr);

    void setPackageJsonObject(const QJsonObject &package_json_object);

    virtual bool initialize(const QStringList& arguments, QString* error_string);
    virtual void pluginsInitialized();
    virtual void aboutToShutDown();

signals:

public slots:

private:
    QJsonObject package_json_object_;
};

}

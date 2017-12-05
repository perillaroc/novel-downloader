#pragma once

#include "package_system_global.h"

#include <QString>
#include <QJsonObject>
#include <QPointer>

namespace PackageSystem{

class PackageInterface;
class PackageManager;

class PACKAGE_SYSTEM_EXPORT PackageSpec
{
public:
    explicit PackageSpec(QPointer<PackageManager> package_manager);

    ~PackageSpec();

    bool read(const QString &file_path);

    QString name() const;
    void setName(const QString &name);

    QString fileLocation() const;

    PackageInterface *plugin();

    void addObject(QObject *obj);
    void removeObject(QObject *obj);

    // Step 1. load
    bool loadLibrary();

    // Step 2. initialize
    bool initializePlugin();

    // Step 3. pluginsInitialized
    bool pluginsInitialized();

private:
    bool readPackageJson(const QJsonObject &package_json);

    QString name_;
    QString category_;
    QJsonObject package_json_object_;

    // file
    QString file_location_;
    QString file_path_;

    // plugin
    QPointer<PackageInterface> package_;

    // package manager
    QPointer<PackageManager> package_manager_;
};

}

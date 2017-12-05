#pragma once

#include "package_system_global.h"

#include <QObject>
#include <QString>
#include <QPointer>
#include <QJsonObject>

namespace PackageSystem{

class PackageSpec;
class PackageManager;

class PACKAGE_SYSTEM_EXPORT PackageInterface : public QObject
{
    Q_OBJECT
public:
    explicit PackageInterface(QPointer<PackageManager> package_manager, QObject *parent = nullptr);
    ~PackageInterface();

    void setPackageJsonObject(const QJsonObject &package_json_object)
    {
        package_json_object_ = package_json_object;
    }

    QSharedPointer<PackageSpec> packageSpec();

    virtual bool initialize(const QStringList& arguments, QString* error_string) = 0;
    virtual void pluginsInitialized() = 0;
    virtual void aboutToShutDown() = 0;

signals:

public slots:

protected:
    QJsonObject package_json_object_;

    // only a reference, may be use another pointer class.
    QSharedPointer<PackageSpec> package_spec_;

private:
    QPointer<PackageManager> package_manager_;
    friend class PackageSpec;
};

}

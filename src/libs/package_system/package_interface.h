#pragma once

#include "package_system_global.h"

#include <QObject>
#include <QString>
#include <QPointer>

namespace PackageSystem{

class PackageSpec;

class PACKAGE_SYSTEM_EXPORT PackageInterface : public QObject
{
    Q_OBJECT
public:
    explicit PackageInterface(QObject *parent = nullptr);
    ~PackageInterface();

    QSharedPointer<PackageSpec> pluginSpec();

    virtual bool initialize(const QStringList& arguments, QString* error_string) = 0;
    virtual void pluginsInitialized() = 0;
    virtual void aboutToShutDown() = 0;

    void addObject(QObject *obj);
    void removeObject(QObject *obj);

signals:

public slots:

private:
    // only a reference, may be use another pointer class.
    QSharedPointer<PackageSpec> plugin_spec_;

    friend class PackageSpec;
};

}

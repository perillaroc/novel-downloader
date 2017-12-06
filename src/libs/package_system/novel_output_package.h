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

    QDir getBaseDir() const;
    QString getMainCommand() const;
    QStringList getBookTypeList() const;
signals:

public slots:

private:
    QDir base_dir_;
    QString main_;

    QStringList book_type_list_;
};

}

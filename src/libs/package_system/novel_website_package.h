#pragma once

#include "package_interface.h"

#include <QDir>
#include <QJsonArray>

namespace PackageSystem{

class PluginManager;

class PACKAGE_SYSTEM_EXPORT NovelWebsitePackage : public PackageInterface
{
    Q_OBJECT
public:
    explicit NovelWebsitePackage(QPointer<PackageManager> package_manager, QObject *parent = nullptr);

    virtual bool initialize(const QStringList& arguments, QString* error_string);
    virtual void pluginsInitialized();
    virtual void aboutToShutDown();

    QDir getBaseDir() const;
    QString getMainCommand() const;
    QString getWebsiteMatcherPattern() const;
    QJsonArray getMenu() const;

signals:

public slots:

private:
    QDir base_dir_;
    QString main_;
    QString website_matcher_pattern_;
    QJsonArray menu_;
};

}

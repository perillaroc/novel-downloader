#include "novel_website_package.h"
#include "package_spec.h"

#include <QtDebug>

using namespace PackageSystem;

NovelWebsitePackage::NovelWebsitePackage(QPointer<PackageManager> package_manager, QObject *parent) :
    PackageInterface{package_manager, parent}
{

}

bool NovelWebsitePackage::initialize(const QStringList &arguments, QString *error_string)
{
    QDir dir{packageSpec()->fileLocation()};
    base_dir_ = dir;

    main_ = package_json_object_["main"].toString();

    QJsonObject contributes_object = package_json_object_["contributes"].toObject();
    QJsonObject website_matcher = contributes_object["website_matcher"].toObject();
    QJsonArray patterns = website_matcher["pattern"].toArray();

    website_matcher_pattern_ = patterns[0].toObject()["regexp"].toString();
    menu_ = contributes_object["menu"].toArray();

    qDebug()<<website_matcher_pattern_;

    package_spec_->addObject(this);

    return true;
}

void NovelWebsitePackage::pluginsInitialized()
{
    return;
}

void NovelWebsitePackage::aboutToShutDown()
{
    return;
}

QDir NovelWebsitePackage::getBaseDir() const
{
    return base_dir_;
}

QString NovelWebsitePackage::getMainCommand() const
{
    return main_;
}

QString NovelWebsitePackage::getWebsiteMatcherPattern() const
{
    return website_matcher_pattern_;
}

QJsonArray NovelWebsitePackage::getMenu() const
{
    return menu_;
}

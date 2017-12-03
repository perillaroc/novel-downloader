#include "novel_website_package.h"

using namespace PackageSystem;

NovelWebsitePackage::NovelWebsitePackage(QObject *parent) :
    PackageInterface{parent}
{

}

void NovelWebsitePackage::setPackageJsonObject(const QJsonObject &package_json_object)
{
    package_json_object_ = package_json_object;
}

bool NovelWebsitePackage::initialize(const QStringList &arguments, QString *error_string)
{
    return true;
}

void NovelWebsitePackage::pluginsInitialized()
{
    return true;
}

void NovelWebsitePackage::aboutToShutDown()
{
    return true;
}

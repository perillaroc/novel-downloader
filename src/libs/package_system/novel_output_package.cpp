#include "novel_output_package.h"
using namespace PackageSystem;

NovelOutputPackage::NovelOutputPackage(QPointer<PackageManager> package_manager, QObject *parent):
    PackageInterface{package_manager, parent}
{

}

bool NovelOutputPackage::initialize(const QStringList &arguments, QString *error_string)
{
    return true;
}

void NovelOutputPackage::pluginsInitialized()
{
    return;
}

void NovelOutputPackage::aboutToShutDown()
{
    return;
}

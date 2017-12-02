#include "package_interface.h"
#include "package_spec.h"
#include "package_manager.h"

using namespace PackageSystem;

PackageInterface::PackageInterface(QObject *parent) :
    QObject{parent},
    plugin_spec_{nullptr}
{

}

PackageInterface::~PackageInterface()
{

}

QSharedPointer<PackageSpec> PackageInterface::pluginSpec()
{
    return plugin_spec_;
}

void PackageInterface::addObject(QObject *obj)
{
    //PackageManager::addObject(obj);
}

void PackageInterface::removeObject(QObject *obj)
{
    //PackageManager::removeObject(obj);
}

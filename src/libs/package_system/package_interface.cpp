#include "package_interface.h"
#include "package_spec.h"
#include "package_manager.h"

using namespace PackageSystem;

PackageInterface::PackageInterface(QPointer<PackageManager> package_manager, QObject *parent) :
    QObject{parent},
    package_spec_{nullptr},
    package_manager_{package_manager}
{

}

PackageInterface::~PackageInterface()
{

}

QSharedPointer<PackageSpec> PackageInterface::packageSpec()
{
    return package_spec_;
}

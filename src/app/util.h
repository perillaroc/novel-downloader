#pragma once

#include <QString>
#include <QPointer>

namespace PackageSystem{
class NovelWebsitePackage;
class NovelOutputPackage;
class PackageManager;
}

namespace Util{


PackageSystem::NovelWebsitePackage *detectNovelWebsitePackage(
        QPointer<PackageSystem::PackageManager> package_manager, const QString &url);

PackageSystem::NovelOutputPackage *detectNovelOutputPackage(
        QPointer<PackageSystem::PackageManager> package_manager, const QString &book_type);

}

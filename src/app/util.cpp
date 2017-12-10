#include "util.h"

#include <QRegularExpression>

#include <package_system/package_manager.h>
#include <package_system/novel_website_package.h>
#include <package_system/novel_output_package.h>

using namespace PackageSystem;

namespace Util{

NovelWebsitePackage *detectNovelWebsitePackage(QPointer<PackageManager> package_manager, const QString &url)
{
    QList<NovelWebsitePackage*> website_packages = package_manager->getObjects<NovelWebsitePackage>();

    foreach(NovelWebsitePackage *website_package, website_packages)
    {
        QString website_matcher_pattern = website_package->getWebsiteMatcherPattern();
        QRegularExpression re(website_matcher_pattern);
        QRegularExpressionMatch match = re.match(url);
        if(match.hasMatch())
        {
            return website_package;
        }
    }
    return nullptr;
}

NovelOutputPackage *detectNovelOutputPackage(QPointer<PackageManager> package_manager, const QString &book_type)
{
    QList<NovelOutputPackage*> packages = package_manager->getObjects<NovelOutputPackage>();

    foreach(NovelOutputPackage *package, packages)
    {
        QStringList book_type_list = package->getBookTypeList();
        if(book_type_list.contains(book_type))
        {
            return package;
        }
    }
    return nullptr;
}

}

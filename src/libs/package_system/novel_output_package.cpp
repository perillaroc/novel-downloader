#include "novel_output_package.h"
#include "package_spec.h"

#include <QJsonArray>

using namespace PackageSystem;

NovelOutputPackage::NovelOutputPackage(QPointer<PackageManager> package_manager, QObject *parent):
    PackageInterface{package_manager, parent}
{

}

bool NovelOutputPackage::initialize(const QStringList &arguments, QString *error_string)
{
    QDir dir{packageSpec()->fileLocation()};
    base_dir_ = dir;

    main_ = package_json_object_["main"].toString();

    QJsonObject contributes_object = package_json_object_["contributes"].toObject();
    QJsonArray book_type_array = contributes_object["book_type"].toArray();
    for (int index = 0; index < book_type_array.size(); ++index)
    {
        QString book_type = book_type_array[index].toString();
        book_type_list_.append(book_type);
    }

    package_spec_->addObject(this);
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

QDir NovelOutputPackage::getBaseDir() const
{
    return base_dir_;
}

QString NovelOutputPackage::getMainCommand() const
{
    return main_;
}

QStringList NovelOutputPackage::getBookTypeList() const
{
    return book_type_list_;
}

#include "package_spec.h"
#include "package_interface.h"
#include "package_manager.h"

#include "novel_website_package.h"
#include "novel_output_package.h"

#include <QPluginLoader>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtDebug>

using namespace PackageSystem;


PackageSpec::PackageSpec(QPointer<PackageManager> package_manager):
    package_{},
    package_manager_{package_manager}
{

}

PackageSpec::~PackageSpec()
{
    if(package_)
        package_->deleteLater();
}


bool PackageSpec::read(const QString &file_path)
{
    QFileInfo file_info(file_path);
    file_location_ = file_info.absolutePath();
    file_path_ = file_info.absoluteFilePath();

    if(!file_info.exists())
    {
        qDebug()<<"[PluginSpec::read] plugin file doesn't exist:"<<file_path;
        return false;
    }

    QFile package_file{file_path};
    if (!package_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("[PluginSpec::read] Couldn't open package json file.");
        return false;
    }
    QByteArray file_content = package_file.readAll();
    QJsonDocument package_json_doc = QJsonDocument::fromJson(file_content);

    package_json_object_ = package_json_doc.object();

    if(!readPackageJson(package_json_object_))
    {
        qDebug()<<"[PluginSpec::read] failed read package json.";
        return false;
    }

    return true;
}

QString PackageSpec::name() const
{
    return name_;
}

void PackageSpec::setName(const QString &name)
{
    name_ = name;
}

QString PackageSpec::fileLocation() const
{
    return file_location_;
}

PackageInterface *PackageSpec::plugin()
{
    return package_;
}

void PackageSpec::addObject(QObject *obj)
{
    package_manager_->addObject(obj);
}

void PackageSpec::removeObject(QObject *obj)
{
    package_manager_->removeObject(obj);
}

bool PackageSpec::loadLibrary()
{
//    if(!loader_.load())
//    {
//        qDebug()<<"[PluginSpec::loadLibrary] Can't load plugin"<<name_;
//        return false;
//    }

//    QObject *a = loader_.instance();
//    PackageInterface *plugin_object = qobject_cast<PackageInterface*>(a);
//    if (!plugin_object)
//    {
//        qDebug()<<"[PluginSpec::loadLibrary] Can't load IPlugin object in plugin"<< name_;
//        loader_.unload();
//        return false;
//    }
//    plugin_ = plugin_object;
//    plugin_->plugin_spec_ = this;
    if(category_ == "py/novel_website")
    {
        qDebug()<<"[PackageSpec::loadLibrary] create novel website package.";
        QPointer<NovelWebsitePackage> package_object = new NovelWebsitePackage{package_manager_};
        package_object->setPackageJsonObject(package_json_object_);

        package_object->package_spec_ = QSharedPointer<PackageSpec>(this);
        package_ = package_object;
    }
    else if(category_ == "py/book_output")
    {
        qDebug()<<"[PackageSpec::loadLibrary] create novel output package.";
        QPointer<NovelOutputPackage> package_object = new NovelOutputPackage{package_manager_};
        package_object->setPackageJsonObject(package_json_object_);

        package_object->package_spec_ = QSharedPointer<PackageSpec>(this);
        package_ = package_object;
    }
    else
    {

    }


    return true;
}

bool PackageSpec::initializePlugin()
{
    QStringList arguments;
    QString error_string;
    if (!package_->initialize(arguments, &error_string)) {
        qDebug()<<"[PluginSpec::pluginsInitialized] failed: "<< error_string;
        return false;
    }
    return true;
}

bool PackageSpec::pluginsInitialized()
{
    package_->pluginsInitialized();
    return true;
}

bool PackageSpec::readPackageJson(const QJsonObject &package_json)
{
    qDebug()<<"[PluginSpec::readPackageJson]";
    qDebug(QJsonDocument(package_json).toJson(QJsonDocument::Indented));
    QJsonValue value;

    value = package_json.value(QLatin1String("name"));
    if(!value.isString())
    {
        qDebug()<<"[PluginSpec::readPackageJson] package name not found.";
        return false;
    }
    name_ = value.toString();

    value = package_json.value(QLatin1String("category"));
    if(!value.isString())
    {
        qDebug()<<"[PluginSpec::readPackageJson] category not found.";
        return false;
    }
    category_ = value.toString();

    return true;
}

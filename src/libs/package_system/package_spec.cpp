#include "package_spec.h"
#include "package_interface.h"

#include <QPluginLoader>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QtDebug>

using namespace PackageSystem;


PackageSpec::PackageSpec():
    plugin_()
{

}

PackageSpec::~PackageSpec()
{
    if(plugin_)
        plugin_->deleteLater();
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

PackageInterface *PackageSpec::plugin()
{
    return plugin_;
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
    return true;
}

bool PackageSpec::initializePlugin()
{
    QStringList arguments;
    QString error_string;
    if (!plugin_->initialize(arguments, &error_string)) {
        qDebug()<<"[PluginSpec::pluginsInitialized] failed: "<< error_string;
        return false;
    }
    return true;
}

bool PackageSpec::pluginsInitialized()
{
    plugin_->pluginsInitialized();
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

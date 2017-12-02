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
//    QFileInfo file_info(file_path);
//    file_location_ = file_info.absolutePath();
//    file_path_ = file_info.absoluteFilePath();

//    if(!file_info.exists())
//    {
//        qDebug()<<"[PluginSpec::read] plugin file doesn't exist:"<<file_path;
//        return false;
//    }

//    if(!readPackageJson(loader_.metaData()))
//    {
//        qDebug()<<"[PluginSpec::read] failed read meta data:"<<loader_.metaData();
//        return false;
//    }

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
//    qDebug()<<"[PluginSpec::readMetaData]";
//    qDebug(QJsonDocument(package_json).toJson(QJsonDocument::Indented));
//    QJsonValue value;

//    value = package_json.value(QLatin1String("MetaData"));
//    if(!value.isObject())
//    {
//        qDebug()<<"[PluginSpec::readMetaData] Plugin MetaData not found.";
//        return false;
//    }
//    QJsonObject plugin_meta_data = value.toObject();

//    value = plugin_meta_data.value(QLatin1String("name"));
//    if(value.isUndefined() || !value.isString())
//    {
//        qDebug()<<"[PluginSpec::readMetaData] name has error.";
//        return false;
//    }
//    name_ = value.toString();

//    value = plugin_meta_data.value(QLatin1String("dependencies"));
//    if(value.isUndefined() || !value.isArray())
//    {
//        qDebug()<<"[PluginSpec::readMetaData] dependencies has error.";
//        return false;
//    }
//    {
//        QJsonArray array = value.toArray();
//        foreach (const QJsonValue &v, array)
//        {
//            if (!v.isObject())
//            {
//                qDebug()<<"[PluginSpec::readMetaData] a dependency entry is not object.";
//                return false;
//            }
//            QJsonObject dependency_object = v.toObject();
//            PackageDependency dep;

//            value = dependency_object.value(QLatin1Literal("name"));
//            if(value.isUndefined() || !value.isString())
//            {
//                qDebug()<<"[PluginSpec::readMetaData] a dependency entry's name has an error.";
//                return false;
//            }
//            dep.name_ = value.toString();

//            dependency_list_.append(dep);
//        }
//    }

    return true;
}

#include "menu_manager.h"
#include "mainwindow.h"

#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QDesktopServices>
#include <QtDebug>

#include <package_system/novel_website_package.h>
#include <package_system/package_manager.h>

using namespace Core;
using namespace PackageSystem;

MenuManager::MenuManager(QPointer<MainWindow> main_window, QObject *parent) :
    QObject{parent},
    main_window_{main_window}
{

}

void MenuManager::setupMenus()
{
    QList<NovelWebsitePackage*> website_packages = main_window_->getPackageManager()->getObjects<NovelWebsitePackage>();

    foreach(NovelWebsitePackage *website_package, website_packages)
    {
        QJsonArray menu_array = website_package->getMenu();
        foreach(QJsonValue menu_value, menu_array)
        {
            QJsonObject menu_object = menu_value.toObject();
            setupSubmenu(menu_object, main_window_->menuBar());
        }
    }
}

void MenuManager::setupSubmenu(QJsonObject menu_object, QPointer<QWidget> parent)
{
    QString menu_label = menu_object["label"].toString();
    qDebug()<<"[MainWindow::setupSubmenu] menu:"<<menu_label;

    if(menu_object.contains("submenu"))
    {
        QPointer<QMenu> menu = findMenu(menu_label, parent);
        if(menu.isNull())
        {
            menu = createMenu(menu_label, parent);
        }
        QJsonArray sub_menus = menu_object["submenu"].toArray();
        foreach(QJsonValue menu_value, sub_menus)
        {
            QJsonObject menu_object = menu_value.toObject();
            setupSubmenu(menu_object, qobject_cast<QWidget*>(menu));
        }
    }
    else
    {
        QPointer<QAction> action = findAction(menu_label, parent);
        if(action.isNull())
        {
            action = createAction(menu_label, parent);
        }
        action->disconnect();
        QString command = menu_object["command"].toString();
        if(command.startsWith("core."))
        {
            QString core_command = command.mid(5);
            if(core_command == "open_website")
            {
                qDebug()<<"[MainWindow::setupSubmenu] find core.open_website.";
                QString url = menu_object["url"].toString();
                connect(action, &QAction::triggered, [=](bool){
                    bool flag = QDesktopServices::openUrl(QUrl(url));
                });
            }
        }
    }
}

QPointer<QMenu> MenuManager::createMenu(const QString &title, QPointer<QWidget> parent)
{
    QPointer<QMenu> menu{new QMenu(parent)};
    menu->setTitle(title);
    QMenu *parent_menu = qobject_cast<QMenu*>(parent.data());
    if(parent_menu)
    {
        parent_menu->addMenu(menu);
    }
    return menu;
}

QPointer<QMenu> MenuManager::findMenu(const QString &title, QPointer<QWidget> parent)
{
    if(parent == nullptr)
    {
        parent = main_window_->menuBar();
    }
    QList<QMenu*> menus = parent->findChildren<QMenu*>();
    foreach(QMenu *menu, menus)
    {
        if(menu->title() == title)
        {
            return QPointer<QMenu>(menu);
        }
    }
    return QPointer<QMenu>();
}

QPointer<QAction> MenuManager::createAction(const QString &text, QPointer<QWidget> parent)
{
    QPointer<QAction> action{new QAction(parent)};
    action->setText(text);
    QMenu *parent_menu = qobject_cast<QMenu*>(parent.data());
    if(parent_menu)
    {
        parent_menu->addAction(action);
    }
    return action;
}

QPointer<QAction> MenuManager::findAction(const QString &text, QPointer<QWidget> parent)
{
    QList<QAction*> actions = parent->findChildren<QAction*>();
    foreach(QAction *action, actions)
    {
        if(action->text() == text)
        {
            return QPointer<QAction>(action);
        }
    }
    return QPointer<QAction>();
}



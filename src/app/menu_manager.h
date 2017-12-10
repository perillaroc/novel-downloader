#pragma once

#include <QObject>
#include <QJsonObject>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QMenu;
class QAction;
class QWidget;
QT_END_NAMESPACE

namespace Core{

class MainWindow;

class MenuManager : public QObject
{
    Q_OBJECT
public:
    explicit MenuManager(QPointer<MainWindow> main_window, QObject *parent = nullptr);

    void setupMenus();

signals:

public slots:

private:
    void setupSubmenu(QJsonObject menu_object, QPointer<QWidget> parent);

    QPointer<QMenu> createMenu(const QString &title, QPointer<QWidget> parent = nullptr);
    QPointer<QMenu> findMenu(const QString &title, QPointer<QWidget>  parent = nullptr);
    QPointer<QAction> createAction(const QString &text, QPointer<QWidget>  parent);
    QPointer<QAction> findAction(const QString &text, QPointer<QWidget>  parent);

    QPointer<MainWindow> main_window_;

};

}

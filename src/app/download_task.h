#pragma once

#include <QString>
#include <QObject>
#include <QPointer>


namespace Core{

class MainWindow;

class DownloadTask: public QObject
{
    Q_OBJECT
public:
    DownloadTask(QPointer<MainWindow> main_window, QObject *parent=nullptr);

    void run();

    QString name_;
    QString link_;
    QString directory_;
    int task_no_;

private:
    QPointer<MainWindow> main_window_;
};

}

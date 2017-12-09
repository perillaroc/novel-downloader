#pragma once

#include <QObject>
#include <QList>
#include <QPointer>
#include <QFutureWatcher>

namespace Core{

class DownloadTask;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);

    void registerWidget(QPointer<QWidget> widget);

    void setDownloadTaskQueue(QVector<QPointer<DownloadTask>> download_tasks);

    void startDownload();
    void setPaused(bool flag);

signals:

public slots:

private:
    QList<QPointer<QWidget>> registered_widgets_;

    QFutureWatcher<void> future_watcher_;

    QVector<QPointer<DownloadTask>> download_task_queue_;
};

}

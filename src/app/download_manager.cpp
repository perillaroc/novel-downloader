#include "download_manager.h"
#include "download_task.h"

#include <QWidget>
#include <QtConcurrent>

using namespace Core;

DownloadManager::DownloadManager(QObject *parent) :
    QObject{parent}
{

}

void DownloadManager::registerWidget(QPointer<QWidget> widget)
{
    registered_widgets_.append(widget);
}

void DownloadManager::setDownloadTaskQueue(QVector<QPointer<DownloadTask> > download_tasks)
{
    download_task_queue_ = download_tasks;
}

void DownloadManager::startDownload()
{
    future_watcher_.setFuture(QtConcurrent::map(download_task_queue_, [this](QPointer<DownloadTask> task){
        task->run();
                              }));
}

void DownloadManager::setPaused(bool flag)
{
    future_watcher_.setPaused(flag);
}

#include "download_task.h"
#include "mainwindow.h"

#include <QProcess>
#include <QtDebug>

#include <package_system/novel_website_package.h>
#include <package_system/package_manager.h>

using namespace Core;
using namespace PackageSystem;

DownloadTask::DownloadTask(QPointer<MainWindow> main_window, QObject *parent):
    QObject{parent},
    main_window_{main_window}
{

}

void DownloadTask::run()
{
    main_window_->slotDownloadChapterStarted(this);

    QString content_url = link_;
    QPointer<QProcess> get_chapter_process = new QProcess;
    QString program = main_window_->getPythonBinPath();

    QPointer<NovelWebsitePackage> detected_package;
    detected_package = main_window_->detectNovelWebsitePackage(content_url);
    if(detected_package.isNull())
    {
        qWarning()<<"[DownloadTask::run] can't find package for url:"<<content_url;
        return;
    }

    QStringList arguments;
    arguments<<detected_package->getBaseDir().absoluteFilePath(detected_package->getMainCommand())
             <<"chapter"
             <<"--url=" + content_url;

    connect(get_chapter_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [this, &get_chapter_process](int exit_code, QProcess::ExitStatus exit_status){
        QByteArray std_out_array = get_chapter_process->readAllStandardOutput();
        QByteArray std_err_array = get_chapter_process->readAllStandardError();

        if(exit_status == QProcess::NormalExit)
        {
            main_window_->slotReceiveGetChapterResponse(this, std_out_array, std_err_array);
        }
        else
        {
            qWarning()<<"[DownloadTask::run] process exit abnormal."<<exit_code<<exit_status;
            qDebug()<<"[DownloadTask::run] std out:\n"
                    <<std_out_array;
            qDebug()<<"[DownloadTask::run] std err out:\n"
                    <<std_err_array;
        }
        get_chapter_process->deleteLater();
    });

    get_chapter_process->start(program, arguments);
    get_chapter_process->waitForFinished();


    qDebug()<<"[DownloadTask::run] run task:"<<content_url;
}

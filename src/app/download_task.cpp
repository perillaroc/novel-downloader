#include "download_task.h"
#include "mainwindow.h"
#include "util.h"

#include <QProcess>
#include <QJsonParseError>
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
    detected_package = Util::detectNovelWebsitePackage(main_window_->getPackageManager(), content_url);
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
            QJsonParseError json_parse_error;
            QJsonDocument doc = QJsonDocument::fromJson(std_out_array, &json_parse_error);

            if(json_parse_error.error != QJsonParseError::NoError)
            {
                qWarning()<<"[DownloadTask::run] parse json string error:"<<json_parse_error.errorString();
                qDebug()<<"[DownloadTask::run] std err out:\n"<<std_err_array;
                return;
            }

            QJsonObject root = doc.object();
            QJsonObject data = root["data"].toObject();
            QJsonObject response = data["response"].toObject();
            QString chapter = response["chapter"].toString();

            QString link = link_;
            QString file_name = link.mid(link.lastIndexOf('/')+1);

            QFileInfo chapter_file_info(QDir(directory_), file_name);

            QFile chapter_file(chapter_file_info.absoluteFilePath());
            if (!chapter_file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                //qWarning()<<"[DownloadTask::run] can't open chapter file:"<<chapter_file_info.absoluteFilePath();
                main_window_->slotReceiveGetChapterResponse(this, false, "can't open chapter file: " + chapter_file_info.absoluteFilePath());
                return;
            }

            QTextStream out(&chapter_file);
            out.setCodec("UTF-8");
            out << chapter;
            chapter_file.close();

            main_window_->slotReceiveGetChapterResponse(this);
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


    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LANG", "zh_CN.UTF-8");
    get_chapter_process->setProcessEnvironment(env);
    get_chapter_process->start(program, arguments);
    get_chapter_process->waitForFinished();


    qDebug()<<"[DownloadTask::run] run task:"<<content_url;
}

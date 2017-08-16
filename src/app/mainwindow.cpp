#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMap>
#include <QStandardItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QtConcurrent/QtConcurrent>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow},
    novel_content_model_{new QStandardItemModel{this}}
{
    qRegisterMetaType<DownloadTask>("DownloadTask");


    ui->setupUi(this);

    ui->novel_content_view->setModel(novel_content_model_);

    connect(ui->get_contents_button, &QPushButton::clicked, this, &MainWindow::slotGetContents);
    connect(ui->select_local_direcotry_button, &QPushButton::clicked, this, &MainWindow::slotSelectLocalDirectory);
    connect(ui->download_button, &QPushButton::clicked, this, &MainWindow::slotDownload);
    connect(ui->pause_button, &QPushButton::toggled, this, &MainWindow::slotPauseDownload);

    connect(this, &MainWindow::signalGetContentsResponseReceived, this, &MainWindow::slotReceiveGetGontentsResponse);
    connect(this, &MainWindow::signalGetChapterResponseReceived, this, &MainWindow::slotReceiveGetChapterResponse);

    QString app_dir = QApplication::applicationDirPath();
    python_env_dir_ = app_dir + "/../vendor/python_env";
    python_bin_path_ = python_env_dir_ + "/python.exe";
    plugin_dir_ = app_dir + "/../plugins";

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotGetContents(bool checked)
{
    QString content_url = ui->content_url->text();
    if(content_url.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "content url must be set.");
        return;
    }

    QPointer<QProcess> get_content_process = new QProcess{this};
    QString program = python_bin_path_;
    QStringList arguments;
    arguments<<plugin_dir_ + "/wutuxs/command.py"
        <<"contents"
        <<"--url=" + content_url;

    connect(get_content_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
          [=](int exitCode, QProcess::ExitStatus exitStatus){
        QByteArray std_out_array = get_content_process->readAllStandardOutput();
        QByteArray std_err_array = get_content_process->readAllStandardError();
        emit signalGetContentsResponseReceived(std_out_array, std_err_array);
        get_content_process->deleteLater();
    });

    get_content_process->start(program, arguments);

}

void MainWindow::slotReceiveGetGontentsResponse(const QByteArray &std_out, const QByteArray &std_err)
{
    QJsonParseError json_parse_error;
    QJsonDocument doc = QJsonDocument::fromJson(std_out, &json_parse_error);

    QJsonObject root = doc.object();
    QJsonObject data = root["data"].toObject();
    QJsonObject response = data["response"].toObject();
    QJsonArray contents = response["contents"].toArray();

    novel_content_model_->clear();
    novel_content_model_->setHorizontalHeaderLabels(QStringList()<<"Chapter"<<"List"<<"Status");
    QStandardItem *root_item = novel_content_model_->invisibleRootItem();

    for(int i=0; i<contents.size(); i++)
    {
        QJsonObject an_entry = contents[i].toObject();
        QString name = an_entry["name"].toString();
        QString link = an_entry["link"].toString();
        root_item->appendRow(
            QList<QStandardItem*>()
                    <<new QStandardItem(name)
                    <<new QStandardItem(link)
                    <<new QStandardItem("Pause")
        );
    }

    ui->novel_content_view->resizeColumnsToContents();
}

void MainWindow::slotReceiveGetChapterResponse(const DownloadTask &task, const QByteArray &std_out, const QByteArray &std_err)
{
    QJsonParseError json_parse_error;
    QJsonDocument doc = QJsonDocument::fromJson(std_out, &json_parse_error);

    QJsonObject root = doc.object();
    QJsonObject data = root["data"].toObject();
    QJsonObject response = data["response"].toObject();
    QString chapter = response["chapter"].toString();

    novel_content_model_->item(task.task_no_, 2)->setText("Complete");

//    qDebug()<<"[MainWindow::slotReceiveGetChapterResponse] chapter:"<<chapter;
}

void MainWindow::slotSelectLocalDirectory(bool checked)
{
    QFileDialog file_dialog{this};
    file_dialog.setFileMode(QFileDialog::Directory);
    QString dir_name;
    if (file_dialog.exec())
    {
        dir_name = file_dialog.selectedFiles().first();
        ui->local_directory->setText(dir_name);
    }
}


void MainWindow::slotDownload(bool checked)
{
    if(ui->local_directory->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a local directory."));
        return;
    }

    if(novel_content_model_->rowCount() == 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Contents are empty. Can't download novel"));
        return;
    }

    download_tasks_.clear();
    for(int i=0; i<novel_content_model_->rowCount(); i++)
    {
        DownloadTask task;
        task.name_ = novel_content_model_->item(i, 0)->text();
        task.link_ = novel_content_model_->item(i, 1)->text();
        task.task_no_ = i;
        novel_content_model_->item(i, 2)->setText("Queue");
        download_tasks_.push_back(task);
    }

    future_watcher_.setFuture(QtConcurrent::map(download_tasks_, [this](const DownloadTask &task){
        novel_content_model_->item(task.task_no_, 2)->setText("Active");
        QString content_url = task.link_;
        QPointer<QProcess> get_chapter_process = new QProcess;
        QString program = python_bin_path_;
        QStringList arguments;
        arguments<<plugin_dir_ + "/wutuxs/command.py"
            <<"chapter"
            <<"--url=" + content_url;

        connect(get_chapter_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
              [=](int exitCode, QProcess::ExitStatus exitStatus){
            QByteArray std_out_array = get_chapter_process->readAllStandardOutput();
            QByteArray std_err_array = get_chapter_process->readAllStandardError();
            emit signalGetChapterResponseReceived(task, std_out_array, std_err_array);
//            qDebug()<<QString(std_err_array);
            get_chapter_process->deleteLater();
        });

        get_chapter_process->start(program, arguments);
        get_chapter_process->waitForFinished();


        qDebug()<<"[MainWindow::slotDownload] run task:"<<content_url;
    }));
}

void MainWindow::slotPauseDownload(bool checked)
{
    future_watcher_.setPaused(checked);
}

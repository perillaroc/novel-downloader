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
#include <QDesktopServices>
#include <QUrl>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow},
    novel_content_model_{new QStandardItemModel{this}}
{
    qRegisterMetaType<DownloadTask>("DownloadTask");

    ui->setupUi(this);

    ui->novel_content_view->setModel(novel_content_model_);

    setupButtons();
    setupActions();

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
        QMessageBox::warning(this, tr("Warning"), tr("Novel's content url must be set."));
        return;
    }

    QString selected_plugin_dir = plugin_dir_ + "/wutuxs";
    QString plugin_command = "command.py";

    QPointer<QProcess> get_content_process = new QProcess{this};
    QString program = python_bin_path_;
    QStringList arguments;
    arguments<<selected_plugin_dir + "/" + plugin_command
            <<"contents"
           <<"--url=" + content_url;

    connect(get_content_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int exit_code, QProcess::ExitStatus exit_status){
        QByteArray std_out_array = get_content_process->readAllStandardOutput();
        QByteArray std_err_array = get_content_process->readAllStandardError();
        if(exit_status == QProcess::NormalExit)
        {
            emit signalGetContentsResponseReceived(std_out_array, std_err_array);
        }
        else
        {
            qWarning()<<"[MainWindow::slotGetContents] process exit abnormal."<<exit_code<<exit_status;
            qDebug()<<"[MainWindow::slotGetContents] std out:\n"
                    <<std_out_array;
            qDebug()<<"[MainWindow::slotGetContents] std err out:\n"
                    <<std_err_array;
        }
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
    QString title = response["title"].toString();
    QString author = response["author"].toString();
    QJsonArray contents = response["contents"].toArray();

    ui->title_line_edit->setText(title);
    ui->author_line_edit->setText(author);

    novel_content_model_->clear();
    novel_content_model_->setHorizontalHeaderLabels(QStringList()<<"Chapter"<<"List"<<"Status");
    QStandardItem *root_item = novel_content_model_->invisibleRootItem();

    for(int i=0; i<contents.size(); i++)
    {
        QJsonObject an_entry = contents[i].toObject();
        QString name = an_entry["name"].toString();
        QString link = an_entry["link"].toString();
        QStandardItem* name_item = new QStandardItem(name);
        QStandardItem* link_item = new QStandardItem(link);
        QStandardItem* status_item = new QStandardItem("Pause");
        status_item->setData(QColor(Qt::gray) ,Qt::DecorationRole);
        root_item->appendRow(
                    QList<QStandardItem*>()
                    <<name_item
                    <<link_item
                    <<status_item
                    );
    }

    ui->novel_content_view->resizeColumnsToContents();
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
    QString local_directory = ui->local_directory->text();
    if(local_directory.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a local directory."));
        return;
    }

    if(novel_content_model_->rowCount() == 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Contents are empty. Can't download novel"));
        return;
    }

    // generate download task
    download_tasks_.clear();
    for(int i=0; i<novel_content_model_->rowCount(); i++)
    {
        DownloadTask task;
        task.name_ = novel_content_model_->item(i, 0)->text();
        task.link_ = novel_content_model_->item(i, 1)->text();
        task.directory_ = local_directory;
        task.task_no_ = i;
        novel_content_model_->item(i, 2)->setText("Queue");
        novel_content_model_->item(i, 2)->setData(QColor(Qt::blue) ,Qt::DecorationRole);
        download_tasks_.push_back(task);
    }


    // create contents json file.
    QJsonArray contents_list;
    foreach(DownloadTask task, download_tasks_)
    {
        QJsonObject task_object;
        task_object["name"] = task.name_;
        task_object["link"] = task.link_;
        QString link = task.link_;
        QString file_name = link.mid(link.lastIndexOf('/')+1);

        QFileInfo chapter_file_info(QDir(task.directory_), file_name);
        task_object["file_path"] = chapter_file_info.absoluteFilePath();
        contents_list.append(task_object);
    }

    QJsonObject contents_object;
    contents_object["contents"] = contents_list;
    contents_object["title"] = ui->title_line_edit->text();
    contents_object["author"] = ui->author_line_edit->text();
    QJsonDocument contents_doc;
    contents_doc.setObject(contents_object);
    QFileInfo contents_file_info(QDir(local_directory), "contents.json");
    QFile contents_file(contents_file_info.absoluteFilePath());
    if (!contents_file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Warning"), tr("Couldn't open contents.json."));
        return;
    }
    QByteArray contents_byte_array = contents_doc.toJson();

    QTextStream contents_file_out(&contents_file);
    contents_file_out.setCodec("UTF-8");
    contents_file_out<<contents_byte_array;
    contents_file.close();

    future_watcher_.setFuture(QtConcurrent::map(download_tasks_, [this](const DownloadTask &task){
        novel_content_model_->item(task.task_no_, 2)->setText("Active");
        novel_content_model_->item(task.task_no_, 2)->setData(QColor(Qt::green) ,Qt::DecorationRole);
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

void MainWindow::slotReceiveGetChapterResponse(const DownloadTask &task, const QByteArray &std_out, const QByteArray &std_err)
{
    QJsonParseError json_parse_error;
    QJsonDocument doc = QJsonDocument::fromJson(std_out, &json_parse_error);

    QJsonObject root = doc.object();
    QJsonObject data = root["data"].toObject();
    QJsonObject response = data["response"].toObject();
    QString chapter = response["chapter"].toString();

    QString link = task.link_;
    QString file_name = link.mid(link.lastIndexOf('/')+1);

    QFileInfo chapter_file_info(QDir(task.directory_), file_name);

    QFile chapter_file(chapter_file_info.absoluteFilePath());
    if (!chapter_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning()<<"[MainWindow::slotReceiveGetChapterResponse] can't open chapter file:"<<chapter_file_info.absoluteFilePath();
        novel_content_model_->item(task.task_no_, 2)->setText("Error");
        novel_content_model_->item(task.task_no_, 2)->setData(QColor(Qt::red) ,Qt::DecorationRole);
        return;
    }

    QTextStream out(&chapter_file);
    out.setCodec("UTF-8");
    out << chapter;
    chapter_file.close();

    novel_content_model_->item(task.task_no_, 2)->setText("Complete");
    novel_content_model_->item(task.task_no_, 2)->setData(QColor(Qt::yellow) ,Qt::DecorationRole);

    //    qDebug()<<"[MainWindow::slotReceiveGetChapterResponse] chapter:"<<chapter;
}

void MainWindow::setupButtons()
{
    connect(ui->get_contents_button, &QPushButton::clicked, this, &MainWindow::slotGetContents);
    connect(ui->select_local_direcotry_button, &QPushButton::clicked, this, &MainWindow::slotSelectLocalDirectory);
    connect(ui->download_button, &QPushButton::clicked, this, &MainWindow::slotDownload);
    connect(ui->pause_button, &QPushButton::toggled, this, &MainWindow::slotPauseDownload);
}

void MainWindow::setupActions()
{
    connect(ui->action_exit, &QAction::triggered, [=](bool){
        this->close();
    });
    connect(ui->action_about, &QAction::triggered, [=](bool){
        QMessageBox::about(this, tr("Novel Downloader"), tr("Created by perillaroc."));
    });

    connect(ui->action_wutuxs_open_website, &QAction::triggered, [=](bool){
         bool flag = QDesktopServices::openUrl(QUrl("http://www.wutuxs.com/"));
         qDebug()<<"[MainWindow::action_wutuxs_open_website] open url:"<<flag;
    });
}

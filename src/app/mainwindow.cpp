#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "download_task.h"
#include "download_manager.h"
#include "menu_manager.h"
#include "util.h"

#include <QStandardItemModel>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>
#include <QtDebug>

#include <package_system/package_manager.h>
#include <package_system/novel_website_package.h>
#include <package_system/novel_output_package.h>

using namespace Core;
using namespace PackageSystem;

const QString BOOK_INFO_JSON_FILE_NAME{"book.json"};

MainWindow::MainWindow(QPointer<PackageSystem::PackageManager> package_manager, QWidget *parent) :
    QMainWindow{parent},
    package_manager_{package_manager},
    menu_manager_{new MenuManager{this}},
    download_manager_{new DownloadManager{this}},
    ui{new Ui::MainWindow},
    novel_content_model_{new QStandardItemModel{this}}
{
    ui->setupUi(this);

    ui->novel_content_view->setModel(novel_content_model_);

    setupButtons();
    setupActions();
    setupMenus();

    connect(this, &MainWindow::signalGetContentsResponseReceived, this, &MainWindow::slotReceiveGetGontentsResponse);

    QString app_dir = QApplication::applicationDirPath();
    python_env_dir_ = app_dir + "/../vendor/python_env";
    python_bin_path_ = python_env_dir_ + "/python.exe";
    packages_dir_ = app_dir + "/../packages";

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

QPointer<PackageManager> MainWindow::getPackageManager()
{
    return package_manager_;
}

QString MainWindow::getPythonBinPath() const
{
    return python_bin_path_;
}

void MainWindow::slotGetContents(bool checked)
{
    QString content_url = ui->content_url->text();
    if(content_url.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Novel's content url must be set."));
        return;
    }

    QPointer<NovelWebsitePackage> detected_package;
    detected_package = Util::detectNovelWebsitePackage(package_manager_, content_url);
    if(detected_package.isNull())
    {
        qWarning()<<"[MainWindow::slotGetContents] can't find package for url:"<<content_url;
        return;
    }

    QPointer<QProcess> get_content_process = new QProcess{this};
    QString program = python_bin_path_;
    QStringList arguments;
    arguments<<detected_package->getBaseDir().absoluteFilePath(detected_package->getMainCommand())
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

    if(json_parse_error.error != QJsonParseError::NoError)
    {
        qWarning()<<"[MainWindow::slotReceiveGetGontentsResponse] parse json string error:"<<json_parse_error.errorString();
        return;
    }

    QJsonObject root = doc.object();
    QJsonObject data = root["data"].toObject();
    QJsonObject response = data["response"].toObject();
    QString title = response["title"].toString();
    QString author = response["author"].toString();
    QJsonArray contents = response["contents"].toArray();

    ui->title_line_edit->setText(title);
    ui->author_line_edit->setText(author);

    novel_content_model_->clear();
    novel_content_model_->setHorizontalHeaderLabels(QStringList()<<tr("Chapter")<<tr("List")<<tr("Status"));
    QStandardItem *root_item = novel_content_model_->invisibleRootItem();

    for(int i=0; i<contents.size(); i++)
    {
        QJsonObject an_entry = contents[i].toObject();
        QString name = an_entry["name"].toString();
        QString link = an_entry["link"].toString();
        QStandardItem* name_item = new QStandardItem(name);
        QStandardItem* link_item = new QStandardItem(link);
        QStandardItem* status_item = new QStandardItem(tr("Pause"));
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

void MainWindow::slotSelectOutputDirectory(bool checked)
{
    QFileDialog file_dialog{this};
    file_dialog.setFileMode(QFileDialog::Directory);
    QString dir_name;
    if (file_dialog.exec())
    {
        dir_name = file_dialog.selectedFiles().first();
        ui->output_directory_line_edit->setText(dir_name);
    }
}


void MainWindow::slotDownload(bool checked)
{
    auto local_directory = ui->local_directory->text();
    if(local_directory.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a local directory."));
        return;
    }
    QDir local_directory_object{local_directory};
    if(!local_directory_object.mkpath(local_directory))
    {
        QMessageBox::warning(this, tr("Warning"), tr("Can't create direcotry:") + local_directory);
        return;
    }

    if(novel_content_model_->rowCount() == 0)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Contents are empty. Can't download novel"));
        return;
    }

    // generate download task
    QVector<QPointer<DownloadTask>> download_tasks;
    for(int i=0; i<novel_content_model_->rowCount(); i++)
    {
        QPointer<DownloadTask> task = new DownloadTask{QPointer<MainWindow>{this}};
        task->name_ = novel_content_model_->item(i, 0)->text();
        task->link_ = novel_content_model_->item(i, 1)->text();
        task->directory_ = local_directory;
        task->task_no_ = i;
        novel_content_model_->item(i, 2)->setText("Queue");
        novel_content_model_->item(i, 2)->setData(QColor(Qt::blue), Qt::DecorationRole);
        download_tasks.push_back(task);
    }


    // create contents json file.
    QJsonArray contents_list;
    foreach(QPointer<DownloadTask> task, download_tasks)
    {
        QJsonObject task_object;
        task_object["name"] = task->name_;
        task_object["link"] = task->link_;
        QString link = task->link_;
        QString file_name = link.mid(link.lastIndexOf('/')+1);

        QFileInfo chapter_file_info(QDir(task->directory_), file_name);
        task_object["file_path"] = chapter_file_info.absoluteFilePath();
        contents_list.append(task_object);
    }

    QJsonObject book_object;
    book_object["contents"] = contents_list;
    book_object["title"] = ui->title_line_edit->text();
    book_object["author"] = ui->author_line_edit->text();
    QJsonDocument contents_doc;
    contents_doc.setObject(book_object);
    QFileInfo contents_file_info{QDir(local_directory), BOOK_INFO_JSON_FILE_NAME};
    QFile contents_file{contents_file_info.absoluteFilePath()};
    if (!contents_file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Warning"), tr("Couldn't open contents.json."));
        return;
    }
    QByteArray contents_byte_array = contents_doc.toJson();

    QTextStream contents_file_out{&contents_file};
    contents_file_out.setCodec("UTF-8");
    contents_file_out<<contents_byte_array;
    contents_file.close();

    download_manager_->setDownloadTaskQueue(download_tasks);
    download_manager_->startDownload();
}

void MainWindow::slotPauseDownload(bool checked)
{
    download_manager_->setPaused(checked);
}

void MainWindow::slotDownloadChapterStarted(QPointer<DownloadTask> task)
{
    novel_content_model_->item(task->task_no_, 2)->setText(tr("Active"));
    novel_content_model_->item(task->task_no_, 2)->setData(QColor(Qt::green) ,Qt::DecorationRole);
}

void MainWindow::slotReceiveGetChapterResponse(QPointer<DownloadTask> task, const QByteArray &std_out, const QByteArray &std_err)
{
    QJsonParseError json_parse_error;
    QJsonDocument doc = QJsonDocument::fromJson(std_out, &json_parse_error);

    if(json_parse_error.error != QJsonParseError::NoError)
    {
        qWarning()<<"[MainWindow::slotReceiveGetChapterResponse] parse json string error:"<<json_parse_error.errorString();
        qDebug()<<"[MainWindow::slotReceiveGetChapterResponse] std err out:\n"<<std_err;
        return;
    }

    QJsonObject root = doc.object();
    QJsonObject data = root["data"].toObject();
    QJsonObject response = data["response"].toObject();
    QString chapter = response["chapter"].toString();

    QString link = task->link_;
    QString file_name = link.mid(link.lastIndexOf('/')+1);

    QFileInfo chapter_file_info(QDir(task->directory_), file_name);

    QFile chapter_file(chapter_file_info.absoluteFilePath());
    if (!chapter_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qWarning()<<"[MainWindow::slotReceiveGetChapterResponse] can't open chapter file:"<<chapter_file_info.absoluteFilePath();
        novel_content_model_->item(task->task_no_, 2)->setText(tr("Error"));
        novel_content_model_->item(task->task_no_, 2)->setData(QColor(Qt::red) ,Qt::DecorationRole);
        return;
    }

    QTextStream out(&chapter_file);
    out.setCodec("UTF-8");
    out << chapter;
    chapter_file.close();

    novel_content_model_->item(task->task_no_, 2)->setText(tr("Complete"));
    novel_content_model_->item(task->task_no_, 2)->setData(QColor(Qt::yellow) ,Qt::DecorationRole);
}

void MainWindow::slotGenerateOutput(bool checked)
{
    QString output_directory = ui->output_directory_line_edit->text();
    if(output_directory.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a output directory."));
        return;
    }

    QString  output_file_name = ui->output_file_name_line_edit->text();
    if(output_file_name.isEmpty())
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please input a output file name."));
        return;
    }

    QString book_type = ui->output_book_type_combo_box->currentText();

    QString local_directory = ui->local_directory->text();
    QFileInfo contents_file_info(QDir(local_directory), BOOK_INFO_JSON_FILE_NAME);
    QFileInfo output_file_info(QDir(output_directory), output_file_name);

    QPointer<NovelOutputPackage> detected_package;
    detected_package = Util::detectNovelOutputPackage(package_manager_, book_type);
    if(detected_package.isNull())
    {
        qWarning()<<"[MainWindow::slotGenerateOutput] can't find package for book type:"<<book_type;
        return;
    }

    QPointer<QProcess> get_content_process = new QProcess{this};
    QString program = python_bin_path_;
    QStringList arguments;
    arguments<<detected_package->getBaseDir().absoluteFilePath(detected_package->getMainCommand())
            <<"make"
            <<"--contents-file=" + contents_file_info.absoluteFilePath()
            <<"--output-file=" + output_file_info.absoluteFilePath();

    connect(get_content_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            [=](int exit_code, QProcess::ExitStatus exit_status){
        QByteArray std_out_array = get_content_process->readAllStandardOutput();
        QByteArray std_err_array = get_content_process->readAllStandardError();
        qDebug()<<std_out_array;
        qDebug()<<std_err_array;
        if(exit_status == QProcess::NormalExit)
        {
            QMessageBox::information(this, tr("Output"), tr("Book is generated."));
        }

        get_content_process->deleteLater();
    });

    get_content_process->start(program, arguments);
}

void MainWindow::setupButtons()
{
    connect(ui->get_contents_button, &QPushButton::clicked, this, &MainWindow::slotGetContents);
    connect(ui->select_local_direcotry_button, &QPushButton::clicked, this, &MainWindow::slotSelectLocalDirectory);
    connect(ui->download_button, &QPushButton::clicked, this, &MainWindow::slotDownload);
    connect(ui->pause_button, &QPushButton::toggled, this, &MainWindow::slotPauseDownload);

    connect(ui->select_output_direcotry_button, &QPushButton::clicked, this, &MainWindow::slotSelectOutputDirectory);
    connect(ui->output_generate_button, &QPushButton::clicked, this, &MainWindow::slotGenerateOutput);
}

void MainWindow::setupActions()
{
    connect(ui->action_exit, &QAction::triggered, [=](bool){
        this->close();
    });
    connect(ui->action_about, &QAction::triggered, [=](bool){
        QMessageBox::about(this, tr("Novel Downloader"), tr("Created by perillaroc."));
    });
}

void MainWindow::setupMenus()
{
    menu_manager_->setupMenus();
}

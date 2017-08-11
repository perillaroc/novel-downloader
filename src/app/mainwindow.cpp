#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStandardItemModel>
#include <QFileDialog>
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow{parent},
    ui{new Ui::MainWindow},
    novel_content_model_{new QStandardItemModel{this}}
{
    ui->setupUi(this);
    connect(ui->get_contents_button, &QPushButton::clicked, this, &MainWindow::slotGetContents);
    connect(ui->select_local_direcotry_button, &QPushButton::clicked, this, &MainWindow::slotSelectLocalDirectory);
    connect(ui->download_button, &QPushButton::clicked, this, &MainWindow::slotDownload);

    QString app_dir = QApplication::applicationDirPath();
    QString python_env_dir = app_dir + "/../vendor/python_env";
    QString python_bin_path = python_env_dir + "/python.exe";

    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotGetContents(bool checked)
{

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

}

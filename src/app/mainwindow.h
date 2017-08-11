#pragma once

#include <QMainWindow>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotGetContents(bool checked=true);

    void slotSelectLocalDirectory(bool checked=true);

    void slotDownload(bool checked=true);

private:
    Ui::MainWindow *ui;

    QPointer<QStandardItemModel> novel_content_model_;
};


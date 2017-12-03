#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QFutureWatcher>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE


namespace PackageSystem{
class PackageManager;
}

namespace Ui {
class MainWindow;
}

struct DownloadTask{
    QString name_;
    QString link_;
    QString directory_;
    int task_no_;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setPackageManager(QPointer<PackageSystem::PackageManager> package_manager);

signals:
    void signalGetContentsResponseReceived(const QByteArray &std_out, const QByteArray &std_err);
    void signalGetChapterResponseReceived(const DownloadTask &task, const QByteArray &std_out, const QByteArray &std_err);

private slots:
    void slotGetContents(bool checked=true);
    void slotReceiveGetGontentsResponse(const QByteArray &std_out, const QByteArray &std_err);

    void slotSelectLocalDirectory(bool checked=true);
    void slotSelectOutputDirectory(bool checked=true);

    void slotDownload(bool checked=true);
    void slotPauseDownload(bool checked=true);
    void slotReceiveGetChapterResponse(const DownloadTask &task, const QByteArray &std_out, const QByteArray &std_err);

    void slotGenerateOutput(bool checked=true);

private:
    void setupButtons();
    void setupActions();

    QString detectPlugin(const QString &url) const;

    Ui::MainWindow *ui;

    QPointer<PackageSystem::PackageManager> package_manager_;

    QPointer<QStandardItemModel> novel_content_model_;

    QString python_env_dir_;
    QString python_bin_path_;
    QString packages_dir_;

    QVector<DownloadTask> download_tasks_;
    QFutureWatcher<void> future_watcher_;
};

#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QFutureWatcher>

QT_BEGIN_NAMESPACE
class QStandardItemModel;
QT_END_NAMESPACE


namespace PackageSystem{
class PackageManager;
class NovelWebsitePackage;
class NovelOutputPackage;
}

namespace Core{
class DownloadTask;
class DownloadManager;
class MenuManager;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QPointer<PackageSystem::PackageManager> package_manager, QWidget *parent = 0);
    ~MainWindow();

    QPointer<PackageSystem::PackageManager> getPackageManager();

    QString getPythonBinPath() const;

signals:
    void signalGetContentsResponseReceived(const QByteArray &std_out, const QByteArray &std_err);

public slots:
    void slotDownloadChapterStarted(QPointer<DownloadTask> task);
    void slotReceiveGetChapterResponse(QPointer<DownloadTask> task, bool is_successed=true, const QString& error_msg="");

private slots:
    void slotGetContents(bool checked=true);
    void slotReceiveGetGontentsResponse(const QByteArray &std_out, const QByteArray &std_err);

    void slotSelectLocalDirectory(bool checked=true);
    void slotSelectOutputDirectory(bool checked=true);

    void slotDownload(bool checked=true);
    void slotPauseDownload(bool checked=true);
    void slotGenerateOutput(bool checked=true);

private:
    void setupButtons();
    void setupActions();
    void setupMenus();

    Ui::MainWindow *ui;

    QPointer<PackageSystem::PackageManager> package_manager_;
    QPointer<MenuManager> menu_manager_;
    QPointer<DownloadManager> download_manager_;

    QPointer<QStandardItemModel> novel_content_model_;

    QString python_env_dir_;
    QString python_bin_path_;
    QString packages_dir_;
};

}

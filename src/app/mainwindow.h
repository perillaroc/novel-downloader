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
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QPointer<PackageSystem::PackageManager> package_manager, QWidget *parent = 0);
    ~MainWindow();

signals:
    void signalGetContentsResponseReceived(const QByteArray &std_out, const QByteArray &std_err);

private slots:
    void slotGetContents(bool checked=true);
    void slotReceiveGetGontentsResponse(const QByteArray &std_out, const QByteArray &std_err);

    void slotSelectLocalDirectory(bool checked=true);
    void slotSelectOutputDirectory(bool checked=true);

    void slotDownload(bool checked=true);
    void slotPauseDownload(bool checked=true);
    void slotDownloadChapterStarted(QPointer<Core::DownloadTask> task);
    void slotReceiveGetChapterResponse(QPointer<Core::DownloadTask> task, const QByteArray &std_out, const QByteArray &std_err);

    void slotGenerateOutput(bool checked=true);

private:
    void setupButtons();
    void setupActions();
    void setupMenus();

    void setupSubmenu(QJsonObject menu, QWidget *parent);

    QPointer<QMenu> createMenu(const QString &title, QPointer<QWidget> parent = nullptr);
    QPointer<QMenu> findMenu(const QString &title, QPointer<QWidget>  parent = nullptr);
    QPointer<QAction> createAction(const QString &text, QPointer<QWidget>  parent);
    QPointer<QAction> findAction(const QString &text, QPointer<QWidget>  parent);

    PackageSystem::NovelWebsitePackage *detectNovelWebsitePackage(const QString &url) const;
    PackageSystem::NovelOutputPackage *detectNovelOutputPackage(const QString &book_type) const;


    Ui::MainWindow *ui;

    QPointer<PackageSystem::PackageManager> package_manager_;
    QPointer<Core::DownloadManager> download_manager_;

    QPointer<QStandardItemModel> novel_content_model_;

    QString python_env_dir_;
    QString python_bin_path_;
    QString packages_dir_;

    friend class Core::DownloadTask;
};

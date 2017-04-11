#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef Q_OS_WIN32
#include "quazip/JlCompress.h"
#endif

#ifdef Q_OS_LINUX
#include "quazip5/JlCompress.h"
#endif


#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QNetworkAccessManager>

#include "ydapi.h"
#include "settings.h"

#define ORGANIZATION_NAME "romanzm"
#define ORGANIZATION_DOMAIN "none"
#define APPLICATION_NAME "QBackup"

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
    void showTaskSettings(QListWidgetItem *item);
    void saveTasks();
    void readTasks();

    void runBackup(QString taskName);
    void timeoutBackupEvent();
    void toggleVisible();
    void uploadOnYD(QString fileName);

    void slotTrayMessage(bool succes);      //succes/error message
    void backupFinished();                  //show message in tray

    void closeEvent(QCloseEvent *event);
    void powerOff();


    void on_actionSettings_triggered();

    void on_actionExit_triggered();

    void on_actionAdd_triggered();

    void on_actionDelete_triggered();

    void on_runBackupButton_clicked();

    void on_pushButton_clicked();

signals:
    void signalTrayMessage(bool succes);
    void startYDBackupSignal(QString fileName);

private:
    void startBackupInThread(QString taskName);
    YDApi *api;
    Ui::MainWindow *ui;
    QSettings qSett;
    Settings settingWindow;
    QSystemTrayIcon *systemTray;
    QMenu *trayMenu;
    QAction *triggVisible;
    QAction *exitApplication;
    QTimer *timer;
    QNetworkAccessManager *manager;
};

#endif // MAINWINDOW_H

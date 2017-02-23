#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>
#include <quazip5/JlCompress.h>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
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
    void showOrHide();
    void closeEvent(QCloseEvent *event);

    void on_actionSettings_triggered();

    void on_actionExit_triggered();

    void on_actionAdd_triggered();

    void on_actionDelete_triggered();

    void on_runBackupButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    QSettings qSett;
    Settings settingWindow;
    QSystemTrayIcon *systemTray;
    QMenu *trayMenu;
    QAction *triggVisible;
    QAction *exitApplication;
    QTimer *timer;
};

#endif // MAINWINDOW_H

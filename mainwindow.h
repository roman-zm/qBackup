#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QListWidgetItem>
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

    void on_actionSettings_triggered();

    void on_actionExit_triggered();

    void on_actionAdd_triggered();

    void on_actionDelete_triggered();

private:
    Ui::MainWindow *ui;
    QSettings qSett;
    Settings settingWindow;
};

#endif // MAINWINDOW_H

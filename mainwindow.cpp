#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tasksettings.h"

#include <QListWidget>
#include <QInputDialog>
#include <quazip5/JlCompress.h>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readTasks();
    connect(ui->backupsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(showTaskSettings(QListWidgetItem*)));
    trayMenu = new QMenu(this);
    triggVisible = new QAction("Show/Hide", this);
    exitApplication = new QAction("Exit", this);
    connect(triggVisible, SIGNAL(triggered()), this, SLOT(showOrHide()));
    connect(exitApplication, SIGNAL(triggered(bool)), qApp, SLOT(quit()));
    trayMenu->addAction(triggVisible);
    trayMenu->addAction(exitApplication);

    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(qApp->applicationDirPath()+QDir::separator()+"Backup.png"));
    systemTray->setVisible(true);
    systemTray->setContextMenu(trayMenu);

    timer = new QTimer(this);
    timer->start(50000);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeoutBackupEvent()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSettings_triggered()
{
    settingWindow.show();
}

void MainWindow::on_actionExit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionAdd_triggered()
{
    QString name;
    bool ok;
    do {
        name = QInputDialog::getText(
                this, tr("Name"),
                tr("Enter new backup name:"),
                QLineEdit::Normal,"",&ok);
    } while(ok && name.isEmpty());

    ui->backupsList->addItem(name);
    saveTasks();

}

void MainWindow::saveTasks()
{
    QStringList names;
    for(int i=0; i<ui->backupsList->count(); i++){
        names.push_back(ui->backupsList->item(i)->text());
    }
    qSett.setValue("tasks", names);
    qSett.sync();
}

void MainWindow::readTasks()
{
    QStringList names = qSett.value("tasks").toStringList();
    for(int i=0; i<names.size(); i++){
        ui->backupsList->addItem(names.at(i));
    }
}

void MainWindow::showTaskSettings(QListWidgetItem *item)
{
    taskSettings *task = new taskSettings(this, item->text());
    //taskSettings task(this, item->text());
    task->show();
}

void MainWindow::on_actionDelete_triggered()
{
//    ui->backupsList->removeItemWidget(ui->backupsList->selectedItems().at(0));
    QString name = ui->backupsList->currentItem()->text();
    qSett.remove(name);
    ui->backupsList->takeItem(ui->backupsList->currentRow());
    saveTasks();
}

void MainWindow::showOrHide()
{
    this->setVisible(!this->isVisible());
}

void MainWindow::runBackup(QString taskName)
{
    bool a;
    a = JlCompress::compressDir(qSett.value(QString("%1/BackupDirName").arg(taskName)).toString()+QDir::separator()+taskName+".zip",
                                qSett.value(QString("%1/DirName").arg(taskName)).toString(),true,QDir::AllDirs);
    if(this->isVisible()){
        if(!a) QMessageBox::critical(this,tr("Error"),tr("Backuping error"));
        else QMessageBox::information(this,tr("Succes"),tr("Backuping succes"));
    } else {
        if(!a) systemTray->showMessage(tr("Error"),tr("Backuping error"),QSystemTrayIcon::Critical);
        else systemTray->showMessage(tr("Succes"),tr("Backuping succes"));
    }

}

void MainWindow::on_runBackupButton_clicked()
{
    runBackup(ui->backupsList->selectedItems().at(0)->text());
}

void MainWindow::on_pushButton_clicked()
{
    for(int i=0; i<ui->backupsList->count(); i++){
        runBackup(ui->backupsList->item(i)->text());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    this->hide();
}

void MainWindow::timeoutBackupEvent()
{
    for(int i=0; i<ui->backupsList->count(); i++){
        QString taskName = ui->backupsList->item(i)->text();
        QTime taskTime = qSett.value(QString("%1/Time").arg(taskName)).toTime();
        if(qSett.value(QString("%1/Enabled").arg(taskName)).toBool()){
            if(taskTime.hour() == QTime::currentTime().hour() && taskTime.minute() == QTime::currentTime().minute()){
                //QMessageBox::information(this, tr("Info"), tr("Running backup: ")+taskName);
                systemTray->showMessage(tr("Info"), tr("Running backup: ")+taskName);
                runBackup(taskName);
            }
        }
    }
}

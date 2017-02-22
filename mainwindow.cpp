#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tasksettings.h"

#include <QListWidget>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readTasks();
    connect(ui->backupsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(showTaskSettings(QListWidgetItem*)));
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
    qApp->exit();
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
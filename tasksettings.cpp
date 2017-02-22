#include "tasksettings.h"
#include "ui_tasksettings.h"

#include <QFileDialog>
#include <QSettings>
#include <QLineEdit>
#include <QGroupBox>
#include <QtWidgets>

taskSettings::taskSettings(QWidget *parent, QString taskName) :
    QDialog(parent),
    ui(new Ui::taskSettings)
{
    ui->setupUi(this);
    ui->groupBox->setTitle(taskName);
    name = taskName;
    update();
}

taskSettings::~taskSettings()
{
    delete ui;
}

void taskSettings::on_getDirectoryButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open dir", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->dirLineEdit->setText(path);
}

void taskSettings::on_getBackupDirButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "Open Backups dir", QDir::homePath(), QFileDialog::ShowDirsOnly);
    ui->backDirLineEdit->setText(path);
}

void taskSettings::update()
{
    ui->dirLineEdit->setText(qSett.value(QString("%1/DirName").arg(name)).toString());
    ui->backDirLineEdit->setText(qSett.value(QString("%1/BackupDirName").arg(name)).toString());
}

void taskSettings::saveSettings()
{
    qSett.setValue(QString("%1/DirName").arg(name), ui->dirLineEdit->text());
    qSett.setValue(QString("%1/BackupDirName").arg(name), ui->backDirLineEdit->text());
    qSett.sync();
}

void taskSettings::on_applyButton_clicked()
{
    saveSettings();
}

void taskSettings::on_buttonBox_accepted()
{
    this->accept();
}

void taskSettings::on_buttonBox_rejected()
{
    this->reject();
}

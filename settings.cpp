#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    ui->zipLine->setText(qSett.value("zipPath").toString());
}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_zipGetFolderButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(
                this, tr("Open zip folder"),QDir::rootPath() ,QFileDialog::ShowDirsOnly);
    ui->zipLine->setText(path);
    qSett.setValue("zipPath", path);
}

void Settings::on_pushButton_clicked()
{
    qSett.sync();
}

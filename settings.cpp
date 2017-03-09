#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QSysInfo>
#include <QCheckBox>

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    if(QSysInfo::WindowsVersion == QSysInfo::WV_None)
        ui->autoStartCheckBox->setEnabled(false);
    ui->zipLine->setText(qSett.value("zipPath").toString());
    ui->autoStartCheckBox->setChecked(qSett.value("autostart").toBool());
    ui->OAuthLineEdit->setText(qSett.value("Token").toString());
    ui->YDcheckBox->setChecked(qSett.value("YDEnabled").toBool());
    ui->powerOffCheckBox->setChecked(qSett.value("PowerOffEnabled").toBool());
    ui->PowerOffTimeEdit->setTime(qSett.value("PowerOffTime").toTime());
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
    qSett.setValue("Token", ui->OAuthLineEdit->text());
    qSett.setValue("YDEnabled", ui->YDcheckBox->isChecked());
    qSett.setValue("zipPath", ui->zipLine->text());
    qSett.setValue("PowerOffEnabled", ui->powerOffCheckBox->isChecked());
    qSett.setValue("PowerOffTime", ui->PowerOffTimeEdit->time());
    qSett.sync();
}

void Settings::on_autoStartCheckBox_toggled(bool checked)
{
    qSett.setValue("autostart", checked);
#ifdef Q_OS_WIN32
    if(checked){
        QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                          QSettings::NativeFormat);
        setting.setValue(APPLICATION_NAME,
                         QDir::toNativeSeparators(QCoreApplication::applicationFilePath()) + " MINIMIZE");
        setting.sync();
    } else {
        QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
                          QSettings::NativeFormat);
        setting.remove(APPLICATION_NAME);
        setting.sync();
    }
#endif
}

void Settings::on_pushButton_2_clicked()
{
    QString link =
            "https://oauth.yandex.ru/authorize?response_type=token&client_id=b646ea711501461cb0cdae42f6a673ee";
    QDesktopServices::openUrl(QUrl(link));
}

void Settings::on_buttonBox_accepted()
{
    on_pushButton_clicked();
    this->accept();
}

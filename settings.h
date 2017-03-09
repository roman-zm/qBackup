#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QSettings>

#define ORGANIZATION_NAME "romanzm"
#define ORGANIZATION_DOMAIN "none"
#define APPLICATION_NAME "QBackup"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private slots:
    void on_zipGetFolderButton_clicked();

    void on_pushButton_clicked();

    void on_autoStartCheckBox_toggled(bool checked);

    void on_pushButton_2_clicked();

    void on_buttonBox_accepted();

private:
    Ui::Settings *ui;
    QSettings qSett;
};

#endif // SETTINGS_H

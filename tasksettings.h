#ifndef TASKSETTINGS_H
#define TASKSETTINGS_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class taskSettings;
}

class taskSettings : public QDialog
{
    Q_OBJECT

public:
    explicit taskSettings(QWidget *parent = 0, QString taskName = "");
    ~taskSettings();

private slots:
    void on_getDirectoryButton_clicked();

    void on_getBackupDirButton_clicked();

    void on_applyButton_clicked();

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    void update();
    void saveSettings();
    QString name;
    QSettings qSett;
    Ui::taskSettings *ui;
};

#endif // TASKSETTINGS_H

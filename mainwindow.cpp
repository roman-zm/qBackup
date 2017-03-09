#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tasksettings.h"

#ifdef Q_OS_WIN32
#include "quazip/JlCompress.h"
#endif

#ifdef Q_OS_LINUX
#include "quazip5/JlCompress.h"
#endif

#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QTimer>
#include <QtConcurrentRun>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFileDialog>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    readTasks();
    connect(ui->backupsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(showTaskSettings(QListWidgetItem*)));
    trayMenu = new QMenu(this);
    triggVisible = new QAction(tr("Show/Hide"), this);
    exitApplication = new QAction(tr("Exit"), this);
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
    connect(timer, SIGNAL(timeout()), this, SLOT(powerOff()));

    connect(this, SIGNAL(signalTrayMessage(bool)), this, SLOT(slotTrayMessage(bool)));

    manager = new QNetworkAccessManager(this);
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
    task->show();
}

void MainWindow::on_actionDelete_triggered()
{
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
    QDir dir = qSett.value(QString("%1/DirName").arg(taskName)).toString();
    QDir backDir = qSett.value(QString("%1/BackupDirName").arg(taskName)).toString();

    if(dir.isReadable() && backDir.isReadable()){
        systemTray->showMessage(tr("Backup"),tr("Starting backup: ") + taskName);
        QtConcurrent::run(this, &MainWindow::startBackupInThread, taskName);
    } else {
        systemTray->showMessage(tr("Error"), tr("Error in task paths"),QSystemTrayIcon::Critical);
    }

}

void MainWindow::startBackupInThread(QString taskName)
{
    bool a = false;

    QString fileName = qSett.value(QString("%1/BackupDirName").arg(taskName)).toString()+
            QDir::separator()+taskName+"-"+QDate::currentDate().toString("dd.MM.yyyy");

    QString tempFileName = fileName + ".zip";

    if(QFile(tempFileName).exists()){
        bool exist = true;
        for(int i=0; exist != false; i++){
            QString buffFileName = fileName+QString("(%1)").arg(i);
            if(!QFile(buffFileName+".zip").exists()){
                exist = false;
                fileName = buffFileName+".zip";
            }
        }
    } else {
        fileName += ".zip";
    }

    a = JlCompress::compressDir(fileName,
                                qSett.value(QString("%1/DirName").arg(taskName)).toString(),
                                true,QDir::AllDirs);

    emit signalTrayMessage(a);
}

void MainWindow::on_runBackupButton_clicked()
{
    if(ui->backupsList->selectedItems().empty())
        QMessageBox::critical(this, tr("Error"), tr("Select task"));
    else
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
                runBackup(taskName);
            }
        }
    }
}

void MainWindow::powerOff()
{
    QTime powerOffTime = qSett.value("PowerOffTime").toTime();
    if(qSett.value("PowerOffEnabled").toBool()){
        if(powerOffTime.hour() == QTime::currentTime().hour() &&
                powerOffTime.minute() == QTime::currentTime().minute()){
#ifdef Q_OS_WIN32
            QProcess::startDetached("shutdown -s -f -t 00");
#endif
#ifdef Q_OS_LINUX
            QProcess::startDetached("shutdown -P now");
#endif
            qApp->quit();
        }
    }
}

void MainWindow::slotTrayMessage(bool succes){
    if(!succes){
        systemTray->showMessage(tr("Error"),tr("Backuping error"),QSystemTrayIcon::Critical);
    } else {
        systemTray->showMessage(tr("Succes"),tr("Backuping succes"));
        //if(qSett.value("YDEnabled").toBool()) uploadOnYD(fileName, taskName);
    }
}

/***********говнокод********************/

//void MainWindow::uploadFile(QString UpUrl)
//{
//    QFile *file = new QFile(QFileDialog::getOpenFileName());
//    if(!file->open(QIODevice::ReadOnly)) qDebug() << "Dont read";
//    QString token = qSett.value("Token").toString();
//    QString url = ui->lineEdit->text();
//    QNetworkRequest request;
//    request.setUrl(QUrl(url));
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
//    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));
////    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
////        request.setRawHeader(QByteArray("path"), QByteArray("disk:/gelska.lol"));
////        request.setRawHeader(QByteArray("url"), QByteArray(UpUrl.toUtf8()));
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
//    //manager->post(request, file->readAll());
//    manager->put(request, file);


//}

//void MainWindow::slotFinished(QNetworkReply* reply)
//{
//    QString answer = QString::fromUtf8(reply->readAll());
//    ui->textEdit->setPlainText(answer);
//    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
////    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
////    QJsonObject jsonObj = jsonDoc.object();
////    qDebug() << jsonObj["href"].toString();
////    uploadFile(jsonObj["href"].toString());

////    ui->textEdit->setPlainText();
//    reply->deleteLater();
//}

//void MainWindow::on_pushButton_2_clicked()
//{
////    QString token = qSett.value("Token").toString();
////    QString url = "https://cloud-api.yandex.net:443/v1/disk/resources?path=disk:";
////    url.append(ui->lineEdit->text());
////    QNetworkRequest request;
////    request.setUrl(QUrl(url));
////    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
////    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));
////    request.setRawHeader(QByteArray("path"), QByteArray("disk:/"));
////    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
////    manager->get(request);
//    uploadFile("lol");
//}

//void MainWindow::on_DownloadButt_clicked()
//{
//    qDebug() << "Download func";
//    QString token = qSett.value("Token").toString();
//    QString url = "https://cloud-api.yandex.net:443/v1/disk/resources/upload?";
//    url.append("path=disk:/test.txt");
//    QNetworkRequest request;
//    request.setUrl(QUrl(url));
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
//    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));

//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
//    manager->get(request);
//}

//void MainWindow::uploadOnYD(QString fileName, QString taskName)
//{

//    QString token = qSett.value("Token").toString();
//    qDebug() << "Download on ya func";
//    QString url = "https://cloud-api.yandex.net:443/v1/disk/resources/upload?path=disk:/";
//    url.append(taskName+QDate::currentDate().toString("dd.MM.yyyy")+".zip");
//    QNetworkRequest request;
//    request.setUrl(QUrl(url));
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
//    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));

//    QNetworkAccessManager *managerTwo = new QNetworkAccessManager();

//    QEventLoop *loop = new QEventLoop(this);
//    connect(managerTwo, SIGNAL(finished()), loop, SLOT(quit()));
//    QNetworkReply *reply = managerTwo->get(request);
//    qDebug() << "Start loop";
//    loop->exec();
//    qDebug() << "Disconnected";

//    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
//    QJsonObject jsonObj = jsonDoc.object();
//    qDebug() << jsonObj["href"].toString();
//    //uploadFileOnYD(jsonObj["href"].toString(), fileName, taskName);


//    url = jsonObj["href"].toString();
//    QFile *file = new QFile(fileName);
//    if(!file->open(QIODevice::ReadOnly)){
//        qDebug() << "Dont read";
//        return;
//    }

//    request.setUrl(QUrl(url));
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
//    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));
////    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
////        request.setRawHeader(QByteArray("path"), QByteArray("disk:/gelska.lol"));
////        request.setRawHeader(QByteArray("url"), QByteArray(UpUrl.toUtf8()));
//    disconnect(managerTwo, SIGNAL(finished()), loop, SLOT(quit()));
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
//    manager->put(request, file);
//}

//void MainWindow::uploadFileOnYD(QString url, QString fileName, QString taskName)
//{
//    QFile *file = new QFile(fileName);
//    if(!file->open(QIODevice::ReadOnly)){
//        qDebug() << "Dont read";
//        return;
//    }
//    QString token = qSett.value("Token").toString();
//    QNetworkRequest request;
//    request.setUrl(QUrl(url));
//    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
//    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));
////    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("text/plain"));
////        request.setRawHeader(QByteArray("path"), QByteArray("disk:/gelska.lol"));
////        request.setRawHeader(QByteArray("url"), QByteArray(UpUrl.toUtf8()));
//    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotFinished(QNetworkReply*)));
//    manager->put(request, file);
//}

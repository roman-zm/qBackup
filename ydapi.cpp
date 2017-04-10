#include "ydapi.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>

YDApi::YDApi(QObject *parent) : QObject(parent)
{

}

void YDApi::setToken(QString _token)
{
    token = _token;
}

void YDApi::upload(QString fileName)
{
    longName = fileName;
    shortName = QFileInfo(longName).fileName();
    uploadFile();
}

void YDApi::uploadFile(/*QString fileName*/)
{
    QString url = "https://cloud-api.yandex.net:443/v1/disk/resources/upload?overwrite=true&path=disk:/backup/";
    url.append(shortName);

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFilePUT(QNetworkReply*)));

    manager->get(request);
}

void YDApi::uploadFilePUT(QNetworkReply *reply)
{
    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(uploadFilePUT(QNetworkReply*)));
    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
    QJsonObject jsonObj = jsonDoc.object();

    QString href = jsonObj["href"].toString();          //PUT upload url
    QFile *file = new QFile(longName);
    if(!file->open(QIODevice::ReadOnly)){
        qDebug() << "Dont read";
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(href));
    request.setRawHeader(QByteArray("Accept"), QByteArray("application/json"));
    request.setRawHeader(QByteArray("Authorization"), QByteArray(token.toUtf8()));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SIGNAL(finished()));

    manager->put(request, file);

}

//void YDApi::checkUpload(QNetworkReply *reply)
//{
//    disconnect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(checkUpload(QNetworkReply*)));
//    qDebug() << reply->readAll();

//    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());
//    QJsonObject jsonObj = jsonDoc.object();


//    qDebug() << jsonObj.keys();

//    QMessageBox msg;
//    if(jsonObj.isEmpty())
//        msg.setText("Empty");
//    else
//        msg.setText("Yes");
//    msg.exec();
//}


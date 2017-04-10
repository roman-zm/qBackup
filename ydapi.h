#ifndef YDAPI_H
#define YDAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QString>

class YDApi : public QObject
{
    Q_OBJECT
public:
    explicit YDApi(QObject *parent = 0);

signals:
    void finished();

public slots:
    void setToken(QString _token);
    void upload(QString fileName);

private slots:
    void uploadFile();
    void uploadFilePUT(QNetworkReply *reply);

    /*****test slots******/
//    void uploadFilePUT(QString href);
//    void checkUpload(QNetworkReply *reply);

private:
    //QString fullFileName;
    QString shortName, longName;
    QString token;
    QNetworkAccessManager *manager = new QNetworkAccessManager();
};

#endif // YDAPI_H

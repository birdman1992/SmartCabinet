#ifndef LOGREPORT_H
#define LOGREPORT_H

#include <QObject>
#include <qnetworkreply.h>
#include <QHttpMultiPart>
#include <QFile>
#include <QDateTime>
#include <QNetworkAccessManager>

class LogReport : public QObject
{
    Q_OBJECT
public:
    explicit LogReport(QNetworkAccessManager *m, QObject *parent = 0);
    void reportStart(QString cabId);

private:
    QNetworkReply* reply;
    QNetworkAccessManager* manager;
    QString cabinetId;

    void tarCabinetFiles(QString cabId);

signals:

public slots:
};

#endif // LOGREPORT_H

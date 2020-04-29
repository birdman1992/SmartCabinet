#ifndef TEMPDEVHUB_H
#define TEMPDEVHUB_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QString>
#include <QTimerEvent>
#include "tempmanager.h"
#include "tempcase.h"

class TempDevHub : public QObject
{
    Q_OBJECT
public:
    explicit TempDevHub(QObject *parent = 0);
    QList<TempCase*> devList();

private:
    QTcpServer* server;
    TempManager* tempMan;
    QMap<QString, TempCase*> map_devs;
    int reportTimerId;

    void tempReport();

protected:
    void timerEvent(QTimerEvent* event);

private slots:
    void newDevice();
    void tempDevIdUpdate(TempCase*);

signals:
    void deviceListUpdate();
    void tempDevClicked(TempCase*);
    void tempDevReport(QByteArray reportWd);

};

#endif // TEMPDEVHUB_H

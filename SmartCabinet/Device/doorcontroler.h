#ifndef DOORCONTROLER_H
#define DOORCONTROLER_H

#include <QObject>
#include <QTcpServer>

class DoorControler : public QObject
{
    Q_OBJECT
public:
    explicit DoorControler(QObject *parent = nullptr);

signals:


private:
    QTcpServer* server;
};

#endif // DOORCONTROLER_H

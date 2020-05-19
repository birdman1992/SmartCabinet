#ifndef RFIDDEVHUB_H
#define RFIDDEVHUB_H

#include <QObject>

class RfidDevHub : public QObject
{
    Q_OBJECT
public:
    explicit RfidDevHub(QObject *parent = nullptr);

signals:

};

#endif // RFIDDEVHUB_H

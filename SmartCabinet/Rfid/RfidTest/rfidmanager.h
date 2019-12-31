#ifndef RFIDMANAGER_H
#define RFIDMANAGER_H

#include <QObject>

class RfidManager : public QObject
{
    Q_OBJECT
public:
    explicit RfidManager(QObject *parent = 0);

signals:

public slots:
};

#endif // RFIDMANAGER_H
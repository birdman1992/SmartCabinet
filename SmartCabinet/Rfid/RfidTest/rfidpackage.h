#ifndef RFIDPACKAGE_H
#define RFIDPACKAGE_H

#include <QObject>

class RfidPackage : public QObject
{
    Q_OBJECT
public:
    explicit RfidPackage(QObject *parent = 0);

signals:

public slots:
};

#endif // RFIDPACKAGE_H
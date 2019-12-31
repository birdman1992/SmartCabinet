#ifndef RFIDREADER_H
#define RFIDREADER_H

#include <QObject>

class RfidReader : public QObject
{
    Q_OBJECT
public:
    explicit RfidReader(QObject *parent = 0);

signals:

public slots:
};

#endif // RFIDREADER_H
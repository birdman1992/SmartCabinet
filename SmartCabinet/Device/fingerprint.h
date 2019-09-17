#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <QObject>
#include <QIODevice>
#include <QByteArray>

class FingerPrint : public QObject
{
    Q_OBJECT
public:
    explicit FingerPrint(QIODevice* ioDev, QObject *parent = 0);

public slots:


signals:

private:
    QByteArray dataCache;

private slots:
    void recvData(QByteArray data);

};

#endif // FINGERPRINT_H

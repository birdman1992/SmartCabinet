#include "fingerprint.h"
#include <QDebug>

FingerPrint::FingerPrint(QIODevice *ioDev, QObject *parent) : QObject(parent)
{

}

void FingerPrint::recvData(QByteArray data)
{
    qDebug()<<"";
}

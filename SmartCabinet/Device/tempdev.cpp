#include "tempdev.h"

TempDev::TempDev(QObject *parent) : QObject(parent)
{
    tempInteger = 0;
    tempDecimals = 0;
    humInteger = 0;
    humDecimals = 0;
    tempLabel = NULL;
    humLabel = NULL;
}

void TempDev::setTempLabel(QLabel *lab)
{
    if(lab != NULL)
        tempLabel = lab;
}

void TempDev::setHumLabel(QLabel *lab)
{
    if(lab != NULL)
        humLabel = lab;
}

void TempDev::recvTempData(QByteArray tempData)
{
    QString tempStr = dataToString(tempInteger, tempDecimals, QString("℃"));
    QString humStr = dataToString(humInteger, humDecimals, QString("%RH"));

    if(tempLabel != NULL)
        tempLabel->setText(tempStr);
    if(humLabel != NULL)
        humLabel->setText(humStr);

    emit updateTempString(tempStr);
    emit updateHumString(humStr);
}

QString TempDev::dataToString(int dataInteger, int dataDecimals, QString unit)
{
    return QString("%1.%2%3").arg(dataInteger).arg(dataDecimals).arg(unit);
}

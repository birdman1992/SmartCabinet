#include "tempdev.h"
#include <QDebug>

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

//03b91acd1a083d05
void TempDev::recvTempData(QByteArray tempData)
{
    if(tempData.size() != 8)
        return;
    tempInteger = tempData[4];
    tempDecimals = tempData[5];
    humInteger = tempData[6];
    humDecimals = tempData[7];

    QString tempStr = dataToString(tempInteger, tempDecimals, QString("℃"));
    QString humStr = dataToString(humInteger, humDecimals, QString("%RH"));

    if(tempLabel != NULL)
        tempLabel->setText(tempStr);
    if(humLabel != NULL)
        humLabel->setText(humStr);

    emit updateTempString(tempStr);
    emit updateHumString(humStr);
    emit updateTemp((float)tempInteger + 0.1 * tempDecimals);
    emit updateHum((float)humInteger + 0.1 * humDecimals);
}

QString TempDev::dataToString(int dataInteger, int dataDecimals, QString unit)
{
    return QString("%1.%2%3").arg(dataInteger).arg(dataDecimals).arg(unit);
}

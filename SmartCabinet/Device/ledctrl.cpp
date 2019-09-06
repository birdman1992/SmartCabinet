#include "ledctrl.h"

LedCtrl::LedCtrl(QObject *parent) : QObject(parent)
{
    ioManager = new GpioApi(this);
    ioManager->addOutIO(GpioApi::J312_L1);qDebug("led1");
    ioManager->addOutIO(GpioApi::J312_L2);qDebug("led1");
    ioManager->addOutIO(GpioApi::J312_L3);qDebug("led1");
    ioManager->addOutIO(GpioApi::J312_L4);qDebug("led1");

    ioManager->setOutIO(GpioApi::J312_L1, 1);qDebug("led2");
    ioManager->setOutIO(GpioApi::J312_L2, 1);qDebug("led2");
    ioManager->setOutIO(GpioApi::J312_L3, 1);qDebug("led2");
    ioManager->setOutIO(GpioApi::J312_L4, 1);qDebug("led2");
}

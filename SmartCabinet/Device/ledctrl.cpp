#include "ledctrl.h"
#include <QDebug>

LedCtrl::LedCtrl(QObject *parent) : QObject(parent)
{
    ioManager = new GpioApi(this);
    ioManager->addOutIO(GpioApi::J312_L1);
    ioManager->addOutIO(GpioApi::J312_L2);
    ioManager->addOutIO(GpioApi::J312_L3);
    ioManager->addOutIO(GpioApi::J312_L4);
    ioManager->addOutIO(GpioApi::J313_FAN);

    ioManager->setOutIO(GpioApi::J312_L1, 0);
    ioManager->setOutIO(GpioApi::J312_L2, 0);
    ioManager->setOutIO(GpioApi::J312_L3, 0);
    ioManager->setOutIO(GpioApi::J312_L4, 0);
    state = 0;

//    ledTimer = new QTimer(this);
//    connect(ledTimer, SIGNAL(timeout()), this, SLOT(ledTimeout()));
//    ledTimer->start(500);
}

void LedCtrl::fanSwitch(bool fanOn)
{
    if(fanOn)
        ioManager->setOutIO(GpioApi::J313_FAN, 1);
    else
        ioManager->setOutIO(GpioApi::J313_FAN, 0);
}

void LedCtrl::ledSwitch(int id, bool ledOn)
{
    qDebug()<<"[led switch]"<<ledOn;
    if(ledOn)
    {
        ioManager->setOutIO(GpioApi::J312_L1, 1);
        ioManager->setOutIO(GpioApi::J312_L2, 1);
        ioManager->setOutIO(GpioApi::J312_L3, 1);
        ioManager->setOutIO(GpioApi::J312_L4, 1);
    }
    else
    {
        ioManager->setOutIO(GpioApi::J312_L1, 0);
        ioManager->setOutIO(GpioApi::J312_L2, 0);
        ioManager->setOutIO(GpioApi::J312_L3, 0);
        ioManager->setOutIO(GpioApi::J312_L4, 0);
    }
}

void LedCtrl::ledTimeout()
{
    state = state?0:1;
    ioManager->setOutIO(GpioApi::J312_L3, state);
}

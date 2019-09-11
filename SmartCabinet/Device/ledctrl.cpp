#include "ledctrl.h"

LedCtrl::LedCtrl(QObject *parent) : QObject(parent)
{
    ioManager = new GpioApi(this);
    ioManager->addOutIO(GpioApi::J312_L1);
    ioManager->addOutIO(GpioApi::J312_L2);
    ioManager->addOutIO(GpioApi::J312_L3);
    ioManager->addOutIO(GpioApi::J312_L4);

    ioManager->setOutIO(GpioApi::J312_L1, 1);
    ioManager->setOutIO(GpioApi::J312_L2, 1);
    ioManager->setOutIO(GpioApi::J312_L3, 1);
    ioManager->setOutIO(GpioApi::J312_L4, 1);
    state = 0;

    ledTimer = new QTimer(this);
    connect(ledTimer, SIGNAL(timeout()), this, SLOT(ledTimeout()));
    ledTimer->start(500);
}

void LedCtrl::ledTimeout()
{
    state = state?0:1;
    ioManager->setOutIO(GpioApi::J312_L3, state);
}

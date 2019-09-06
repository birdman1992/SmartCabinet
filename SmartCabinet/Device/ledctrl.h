#ifndef LEDCTRL_H
#define LEDCTRL_H

#include <QObject>
#include "gpio/gpioapi.h"

class LedCtrl : public QObject
{
    Q_OBJECT
public:
    explicit LedCtrl(QObject *parent = 0);
public slots:

signals:

private:
    GpioApi* ioManager;


};

#endif // LEDCTRL_H

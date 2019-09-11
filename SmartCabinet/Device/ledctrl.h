#ifndef LEDCTRL_H
#define LEDCTRL_H

#include <QObject>
#include <QTimer>
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
    QTimer* ledTimer;
    int state;

private slots:
    void ledTimeout();

};

#endif // LEDCTRL_H
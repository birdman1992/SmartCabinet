#include "secwatch.h"
#include <QDebug>

SecWatch* SecWatch::w = new SecWatch;
QTime* SecWatch::t = new QTime;

SecWatch::SecWatch()
{

}

void SecWatch::start()
{
    SecWatch::t->start();
}

void SecWatch::elapsed(QString tag)
{
    qDebug()<<QString("[SecWatch] %1:%2ms").arg(tag).arg(t->elapsed());
}

void SecWatch::restart()
{
    t->restart();
}

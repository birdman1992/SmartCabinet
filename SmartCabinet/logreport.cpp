#include "logreport.h"

LogReport::LogReport(QNetworkAccessManager* m, QObject *parent) : QObject(parent)
{
    manager = m;
}

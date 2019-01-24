#ifndef ROUTEREPAIR_H
#define ROUTEREPAIR_H

#include <QObject>
#include <QProcess>
#include <QRegExp>
#include <QSettings>

class RouteRepair : public QObject
{
    Q_OBJECT
public:
    explicit RouteRepair(QObject *parent = 0);

private:
    QProcess* process;
    QString getDefaultGateway();
signals:

public slots:
    void repairStart(bool);
private slots:
    void recvCheckRst();
};

#endif // ROUTEREPAIR_H

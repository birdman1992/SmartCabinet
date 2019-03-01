#ifndef ROUTEREPAIR_H
#define ROUTEREPAIR_H

#include <QObject>
#include <QProcess>
#include <QRegExp>
#include <QSettings>
#include <QTimer>

class RouteRepair : public QObject
{
    Q_OBJECT
public:
    explicit RouteRepair(QObject *parent = 0);

private:
    QProcess* process;
    QString getDefaultGateway();
    bool isRepairing;

signals:
    void repairOk();

public slots:
    void repairStart(bool);
private slots:
    void recvCheckRst();
    void repair();
};

#endif // ROUTEREPAIR_H

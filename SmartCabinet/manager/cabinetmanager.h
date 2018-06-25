#ifndef CABINETMANAGER_H
#define CABINETMANAGER_H

#include <QObject>

class CabinetManager : public QObject
{
    Q_OBJECT
public:
    CabinetManager* manager();

private:
    explicit CabinetManager(QObject *parent = NULL);
    static CabinetManager* m;

signals:

public slots:
};

#endif // CABINETMANAGER_H

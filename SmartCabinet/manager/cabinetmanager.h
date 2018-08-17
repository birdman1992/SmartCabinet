#ifndef CABINETMANAGER_H
#define CABINETMANAGER_H

#include <QObject>
#include <QPoint>

class CabinetManager : public QObject
{
    Q_OBJECT
public:
    static CabinetManager* manager();
    QString cabMap;
    QString cabLayout;
    QPoint scrPos;
    QString departName;
    QString hospitalName;
    void saveConfig();
    void readConfig();
    void insertCol(int colPos, QString layout);
    void insertUndo();

    /*SETS*/
    void setCabMap(QString map);
    void setCabLayout(QString layout);
    void setScrPos(QPoint pos);
    void setDepartName(QString name);
    void setHospitalName(QString name);

    /*GETS*/
    QString getCabMap();
    QString getCabLayout();
    QPoint getScrPos();
    QString getDepartName();
    QString getHospitalName();

private:
    explicit CabinetManager(QObject *parent = NULL);
    static CabinetManager* m;
    QString configPath;

    void setConfig(QString key, QVariant value);
    QVariant getConfig(QString key, QVariant defaultRet);
    void removeConfig(QString path);
signals:

public slots:
};

#endif // CABINETMANAGER_H

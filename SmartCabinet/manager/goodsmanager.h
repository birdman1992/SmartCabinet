#ifndef GOODSMANAGER_H
#define GOODSMANAGER_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QVariant>
#include <QSettings>

class GoodsManager : public QObject
{
    Q_OBJECT
public:
    static GoodsManager* manager();
    /*SETS*/
    void addGoodsCodes(QString goodsId, QStringList codes);

    /*GETS*/
    QStringList getGoodsCodes(QString goodsId);
    QString getGoodsByCode(QString code);
    QStringList getGoodsList();

    void removeCode(QString code);


private:
    explicit GoodsManager(QObject *parent = NULL);
    static GoodsManager* m;
    QString configPath;
    QMap<QString, QString> codeMap;

    void updateCodeMap();
    void addCodeMap(QString goodsId, QStringList codes);

    void setConfig(QString key, QVariant value);
    QVariant getConfig(QString key, QVariant defaultRet);
    void removeConfig(QString path);
signals:

public slots:
};

#endif // GOODSMANAGER_H

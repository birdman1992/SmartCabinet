#ifndef CABINETCONFIG_H
#define CABINETCONFIG_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QList>
#include "Structs/userinfo.h"
#include "Structs/cabinetinfo.h"
#include "Cabinet/cabinet.h"
#include "medinf.h"

class CabinetConfig
{
public:
    CabinetConfig();
    ~CabinetConfig();
    bool isFirstUse();//判断是否第一次使用
    void addUser(UserInfo* info);
    void creatCabinetConfig(QByteArray);

    QList<UserInfo*> list_user;
    QList<Cabinet*> list_cabinet;
//    QList<>

private:
    bool firstUse;//第一次使用
    int userNum;//用户数量

    void readUserConfig();//读取用户配置
    void readCabinetConfig();//读取柜子配置
    void addNewUser(UserInfo* info);

public slots:


};

#endif // CABINETCONFIG_H

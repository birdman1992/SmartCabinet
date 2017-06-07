#ifndef CABINETCONFIG_H
#define CABINETCONFIG_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QList>
#include "Structs/userinfo.h"
#include "Structs/cabinetinfo.h"
#include "Structs/caseaddress.h"
#include "Cabinet/cabinet.h"
#include "medinf.h"

enum CabState
{
    STATE_NO = 0,//无状态
    STATE_STORE = 1,//存状态
    STATE_FETCH = 2,//取状态
};

class CabinetConfig
{
public:
    CabinetConfig();
    ~CabinetConfig();
    void setCabinetId(QString id);
    QString getCabinetId();
    bool isFirstUse();//判断是否第一次使用
    void addUser(UserInfo* info);//添加用户
    int checkUser(QString userId);//检查用户是否存在，管理员用户返回0,其他用户大于0,不存在小于0
    void creatCabinetConfig(QByteArray);//创建智能柜配置文件
//    void writeCabinetConfig(int cabSeq, int caseIndex, CabinetInfo* info);
    CaseAddress checkCabinetByName(QString name);//根据名字搜索柜格位置，如未搜索到，返回坐标为-1
    int getLockId(int seq, int index);

    QList<UserInfo*> list_user;
    QList<Cabinet*> list_cabinet;
    CabState state;

//    QList<>

private:
    bool firstUse;//第一次使用
    int userNum;//用户数量
//    QString cabId;
    QString cabinetId;

    void readUserConfig();//读取用户配置
    void readCabinetConfig();//读取柜子配置
    void addNewUser(UserInfo* info);
    QChar getPyCh(QString str);

public slots:


};

#endif // CABINETCONFIG_H

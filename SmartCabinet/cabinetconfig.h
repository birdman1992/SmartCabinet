#ifndef CABINETCONFIG_H
#define CABINETCONFIG_H

#include <QObject>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QList>
#include <QTime>
#include "Structs/userinfo.h"
#include "Structs/cabinetinfo.h"
#include "Structs/caseaddress.h"
#include "Cabinet/cabinet.h"
#include "Device/voiceplayer.h"
#include "medinf.h"
#include "manager/lockmanager.h"

enum CabState
{
    STATE_NO = 0,//无状态
    STATE_STORE = 1,//存状态
    STATE_FETCH = 2,//取状态
    STATE_REFUN = 3,//退货状态
    STATE_LIST = 4,//列表取货状态
    STATE_CHECK = 5,//盘点状态
    STATE_REBIND = 6,//重新绑定
    STATE_SPEC = 7,//配置特殊柜
    CMD_CHECK_SHOW = 8,//显示盘点单
    CMD_DAY_REPORT_SHOW = 9,//显示结算单
    STATE_BACK = 10,//还货
};

enum FuncWord{
    funcStore = 0,
    funcFetch = 1,
    funcBack = 2,
    funcRefun = 4,
    funcCheck = 8,
};

class CabinetConfig
{
public:
    ~CabinetConfig();
    static CabinetConfig* config();
    void configInit();
    void saveFetchList(QByteArray _data);
    void readCabinetConfig();//读取柜子配置
    void setSpecialCase(QPoint pos, bool needRecover = true);
    QPoint getSpecialCase();
    QString getSecondUser();
    void setSecondUser(QString userId);
    void setStoreMode(bool needScanAll);
    bool getStoreMode();//return need scanALL
    //depart name
    void setDepartName(QString dptName);
    QString getDepartName();
    QString getApiProName();//获取接口项目名称,默认 spd-web
    void setApiProName(QString apiName);//设置接口项目名称
    int getFuncWord();
    void setFuncWord(int funcWord);

    QString getCabinetLayout();
    QString getCabinetColMap();
    void setCabinetType(int type);
    int getCabinetType();
    QPoint getScreenPos();
    QString getScreenConfig();
    QList<QByteArray> getFetchList();
    void setCabinetId(QString id);
    QString getCabinetMode();
    void setCabinetMode(QString cMode);
    void setScreenPos(int col, int row);
    void setCabLayout(QString layout);
    void showMsg(QString msg, bool iswarnning);
    int getGoodsType(QString packageId);
    void clearOptId();//清除操作者记录
    void setOptId(QString id);//设置操作者id
    QString getOptId();//获取操作者卡号
    QString getPyCh(QString str);
    QString getCabinetId();
    QString getServerAddress();
    QString getServerIp();
    void insertGoods(Goods* info, int row, int col);
    void syncGoods(Goods* info, int row, int col);
    void setServerAddress(QString addr);
    int getUncheckCaseNum();
    int getSysVolem();
    void setSysVolem(int vol);
    int volTodB(int vol);//音量转分贝
    void clearConfig();
    void clearCabinet();
    void wakeUp(int minutes);//唤醒
    int reboot();
    void clearTimeoutFlag();
    int getSleepFlag();
    QString getCurVersion();
    void setCurVersion(QString version);
    void setCardReaderState(bool ok);
    bool getCardReaderState();
    void setCodeScanState(bool ok);
    bool getCodeScanState();
    bool isScreen(int seq, int _index);
    bool isSpec(int seq, int index);
    bool sleepFlagTimeout();//唤醒超时检查
    bool isFirstUse();//判断是否第一次使用
    void addUser(UserInfo* info);//添加用户
    int checkUser(QString userId);//检查用户是否存在，管理员用户返回0,其他用户大于0,不存在小于0
    UserInfo* checkUserLocal(QString userId);//本地用户校验
    bool checkManagers(QString userId);//管理员校验
//    void creatCabinetConfig(QByteArray);//创建智能柜配置文件
    void creatCabinetConfig(QStringList cabLayout, QPoint screenPos);
    void clearGoodsConfig();
//    void writeCabinetConfig(int cabSeq, int caseIndex, CabinetInfo* info);
//    CaseAddress checkCabinetByName(QString name);//根据名字搜索柜格位置,如未搜索到，返回坐标为-1
//    CaseAddress checkCabinetByBarCode(QString id);//根据物品条码搜索物品位置,如未搜索到，返回坐标为-1
//    CaseAddress checkCabinetByGoodsId(QString id);//根据物品条码搜索物品位置,如未搜索到，返回坐标为-1
    int getLockId(int seq, int index);
    int getCaseWidth();
    void removeConfig(CaseAddress addr);
    void setConfig(CaseAddress addr, Goods* info);
    QByteArray creatCabinetJson();
    QByteArray getCabinetPos();
    QByteArray getCabinetSize();

    void searchByPinyin(QString ch);
    void clearSearch();
    QChar str2py(QChar ch);

    QList<UserInfo*> list_user;
    QList<Cabinet*> list_cabinet;
    CabState state;
    VoicePlayer cabVoice;
    QString regId;
    QLabel* msgLab;
    bool netState;
//    QList<>

    QString scanDataTrans(QString code);
    void setCabinetColMap(QString map);
    void setLockCtrl(int cabSeq, int cabIndex, int ctrlSeq, int ctrlIndex);
private:
    CabinetConfig();
    static CabinetConfig* c;
    bool cardReaderIsOk;
    bool codeScanIsOk;
    bool firstUse;//第一次使用
    int sleepFlag;//休眠标志
    int timeoutFlag;//超时标志
    int userNum;//用户数量
    QPoint screenPos;
    QPoint specPos;
    int caseWidth;
//    QString cabId;
    int cabinetType;
    QString curVersion;
    QString cabinetId;
    QString cabinetMode;
    QString serverAddr;
    QString serverIp;
    QString optName;//操作者卡号
    QString secOpt;//第二操作人
    LockManager* lockManager;

    void readUserConfig();//读取用户配置

    void addNewUser(UserInfo* info);
    void restart();//重启自身
    QString initColMap(int cabNum);

public slots:


};

#endif // CABINETCONFIG_H

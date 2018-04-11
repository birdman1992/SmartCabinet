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

enum CabState
{
    STATE_NO = 0,//无状态
    STATE_STORE = 1,//存状态
    STATE_FETCH = 2,//取状态
    STATE_REFUN = 3,//退货状态
    STATE_LIST = 4,//列表取货状态
    STATE_CHECK = 5,//盘点状态
    STATE_REBIND = 6,//重新绑定
};

class CabinetConfig
{
public:
    CabinetConfig();
    ~CabinetConfig();
    void saveFetchList(QByteArray _data);
    QList<QByteArray> getFetchList();
    void setCabinetId(QString id);
    void setScreenPos(int col, int row);
    void showMsg(QString msg, bool iswarnning);
    int getGoodsType(QString packageId);
    void clearOptId();//清除操作者记录
    void setOptId(QString id);//设置操作者id
    QString getOptId();//获取操作者卡号
    QString getPyCh(QString str);
    QString getCabinetId();
    QString getServerAddress();
    void insertGoods(GoodsInfo* info, int row, int col);
    void syncGoods(GoodsInfo* info, int row, int col);
    void setServerAddress(QString addr);
    int getSysVolem();
    void setSysVolem(int vol);
    int volTodB(int vol);//音量转分贝
    void clearConfig();
    void clearCabinet();
    void wakeUp(int minutes);//唤醒
    int reboot();
    void clearTimeoutFlag();
    int getSleepFlag();
    void setCardReaderState(bool ok);
    bool getCardReaderState();
    void setCodeScanState(bool ok);
    bool getCodeScanState();
    bool isScreen(int seq, int _index);
    bool sleepFlagTimeout();//唤醒超时检查
    bool isFirstUse();//判断是否第一次使用
    void addUser(UserInfo* info);//添加用户
    int checkUser(QString userId);//检查用户是否存在，管理员用户返回0,其他用户大于0,不存在小于0
    UserInfo* checkUserLocal(QString userId);//本地用户校验
    bool checkManagers(QString userId);//管理员校验
//    void creatCabinetConfig(QByteArray);//创建智能柜配置文件
    void creatCabinetConfig(QStringList cabLayout, QPoint screenPos);
//    void writeCabinetConfig(int cabSeq, int caseIndex, CabinetInfo* info);
    CaseAddress checkCabinetByName(QString name);//根据名字搜索柜格位置,如未搜索到，返回坐标为-1
    CaseAddress checkCabinetByBarCode(QString id);//根据物品条码搜索物品位置,如未搜索到，返回坐标为-1
    CaseAddress checkCabinetByGoodsId(QString id);//根据物品条码搜索物品位置,如未搜索到，返回坐标为-1
    int getLockId(int seq, int index);
    int getCaseWidth();
    void removeConfig(CaseAddress addr);
    void setConfig(CaseAddress addr, GoodsInfo* info);
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
//    QList<>

    QString scanDataTrans(QString code);
private:
    bool cardReaderIsOk;
    bool codeScanIsOk;
    bool firstUse;//第一次使用
    int sleepFlag;//休眠标志
    int timeoutFlag;//超时标志
    int userNum;//用户数量
    QPoint screenPos;
    int caseWidth;
//    QString cabId;
    QString cabinetId;
    QString serverAddr;
    QString optName;//操作者卡号

    void readUserConfig();//读取用户配置
    void readCabinetConfig();//读取柜子配置
    void addNewUser(UserInfo* info);
    void restart();//重启自身

public slots:


};

#endif // CABINETCONFIG_H

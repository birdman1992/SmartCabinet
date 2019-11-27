#ifndef FINGERPRINT_H
#define FINGERPRINT_H

#include <QWidget>
#include <QByteArray>
#include <QMap>
#include <QMultiMap>
#include <QList>
#include <QStringList>
#include <QButtonGroup>
#include "qsocketcan.h"
#include "cmdpack.h"
#include "userprintmanager.h"

namespace Ui {
class FingerPrint;
}

class FingerPrint : public QWidget
{
    Q_OBJECT

public:
    explicit FingerPrint(QWidget *parent = 0);
    ~FingerPrint();
    void addUser();
    enum LED{
        LED_B=1,
        LED_R=2,
        LED_G=4
    };
    enum MODEL_STATE{
        MODEL_CONFIG=LED_B,
        MODEL_WAIT=LED_R|LED_G,//b
        MODEL_ACTIVE=LED_B|LED_G,//r
        MODEL_NORMAL=LED_R|LED_B,//G
        MODEL_PASS=LED_G,
        MODEL_ERROR=LED_R
    };
    enum CUR_STATE{
        STATE_REG,//注册
        STATE_CHECK//验证
    };

public slots:
    void recvFingerData(int canId, QByteArray data);
    void recvCurCardId(QByteArray cardId);

signals:
    void requireOpenLock(int seq, int index);
    void cmdRet();
    void doorState(int id, bool isOpen);
    void fingerPrintPass(int canId, int fingerId);
    void userCardActive(QByteArray cardId);

private slots:
    void ledStateChanged(int state);
    void moduleActived(int id);
    void doorStateChanged(int id, bool isOpen);
    void userCheckPass(int canId, int fingerId);
    void canDevScan();//扫描所有can设备
    void on_led_r_clicked(bool checked);
    void on_led_g_clicked(bool checked);
    void on_led_b_clicked(bool checked);
    void on_led_off_clicked();
    void on_led_r_2_clicked(bool checked);
    void on_led_g_2_clicked(bool checked);
    void on_led_b_2_clicked(bool checked);
    void on_reg_clicked();
    void on_spin_can_valueChanged(int arg1);
    void on_spin_seq_valueChanged(int arg1);
    void on_spin_index_valueChanged(int arg1);
    void on_save_config_clicked();

private:
    Ui::FingerPrint *ui;
    QSocketCan* socketCan;
    UserPrintManager* manager_user;
    QString curUserCard;
    QString curUserName;
    QByteArray ctrlSeq;
    QByteArray ctrlIndex;
    int curFingerId;
    QMap<int, QString> retCodeList;
    QList<int> canDevList;//CAN设备列表
    QButtonGroup btnState;
    QMultiMap<int, QByteArray*> cmdCache;
    CUR_STATE curState;
    int img_w;
    int img_h;
    int boardCastOut;//发出的广播包
    int boardCastIn;//收回的广播包
    QByteArray img_data;
    quint16 temp_size;
    QByteArray temp_data;
    char generateStep;//注册步骤0-2,区分采集的3张图像
    char ledState;
    char curModule;
    bool waitSelCurModule;
    int curDev;

    void initRetCodeList();
    void showMsg(QString msg);
    void addCmdCache(int id, QByteArray* cmd);
    void sendCmdCache();
    QByteArray* getCmdCache(int id);
    QByteArray dataCheckSum(QByteArray inData);
    void cmdScanDev(int id);//扫描设备id
    void cmdGetSN(int id);//获取设备序列号
    void cmdGetTempId(int id);//获取可用指纹ID
    void cmdGetImage(int id);//采集指纹图像
    void cmdReadImage(int id, char imgType=0);//读取指纹图像
    void cmdGenerate(int id);//产生指纹模板
    void cmdMerge(int id, int mergeNum=3, int bufferId=0);//合成指纹模板
    void cmdReadTemplate(int id, int bufferId=0);//读取指纹模板
    void cmdWriteTemplate(int id, QByteArray _data=QByteArray(), int bufferId=0, bool isDataPack=false);//写入到指纹模板
    void templateDistr(QByteArray _data);//指纹模板分发
    void cmdStoreChar(int id, int tempID, int bufferId=0);//tempID->fingerID
    void cmdDeleteChar(int id, int rangeMin, int rangeMax);
    void cmdSetLed(int id, char state, char led);//指纹模块id，显示状态，LED1,LED2,次数
    void cmdSearch(int id, int bufferId=0, int rangeMin=1, int rangeMax=200);
};

#endif // FINGERPRINT_H

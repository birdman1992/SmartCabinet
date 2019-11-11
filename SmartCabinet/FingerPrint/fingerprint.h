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
        MODEL_WAIT=LED_R|LED_G,
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

private slots:
    void ledStateChanged(int state);
    void moduleActived(int id);
    void on_test_clicked();
    void on_test_upload_clicked();
    void on_test_download_clicked();
    void on_led_r_clicked(bool checked);
    void on_led_g_clicked(bool checked);
    void on_led_b_clicked(bool checked);
    void on_led_off_clicked();

private:
    Ui::FingerPrint *ui;
    QSocketCan* socketCan;
    QString curUserCard;
    QString curFingerId;
    QMap<int, QString> retCodeList;
    QButtonGroup btnState;
    QMultiMap<int, QByteArray*> cmdCache;
    CUR_STATE curState;
    char generateStep;//注册步骤0-2,区分采集的3张图像
    char ledState;
    char curModule;
    bool waitSelCurModule;

    void initRetCodeList();
    void addCmdCache(int id, QByteArray* cmd);
    QByteArray* getCmdCache(int id);
    void cmdGetTempId(int id);//获取可用指纹ID
    void cmdGetImage(int id);
    void cmdGenerate(int id);//产生指纹模板
    void cmdSetLed(int id, char state, char led);//指纹模块id，显示状态，LED1,LED2,次数
};

#endif // FINGERPRINT_H

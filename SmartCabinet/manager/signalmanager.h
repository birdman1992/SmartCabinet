#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include <QObject>
#include "defines.h"
#include "Rfid/RfidTest/rfidmanager.h"

class SignalManager : public QObject
{
    Q_OBJECT
public:
    static SignalManager* manager();

signals:
    void epcAccess(QStringList epcs, UserOpt optType);
    void epcAccess(QStringList fetchEpcs, QStringList backEpcs,QString optNo);
    void epcInfoUpdate();
    void epcConsumeCheck(QStringList);
    void epcConsumed(QStringList epcs);
    void epcStore(QVariantMap);
    void rfidOptRst(bool success, QString msg);
    void doorState(int id, bool isOpen);
    void accessSuccess(QString msg);//存取成功信息
    void accessFailed(QString msg);//存取失败信息
    void updateLoginState(bool);//CabinetWidget::loginStateChanged->
    void configRfidDevice();//rfid设备配置请求,CabinetService::rfidConfig()->FrmRfid::configDevice
    //手术单信息获取
    void requireUpdateOperation();//请求更新手术单 Operation::updateSelOperation
    void operationInfoUpdate();//手术单信息更新 CabinetServer::operationInfoUpdate
    //device sig
    void lockState(int id, bool isOpen);
    void cameraCapture();//触发相机拍摄
    void cameraSnapshot(QString msg);//触发抓拍事件


private:
    explicit SignalManager(QObject *parent = NULL);
    static SignalManager* m;

};

#endif // SIGNALMANAGER_H

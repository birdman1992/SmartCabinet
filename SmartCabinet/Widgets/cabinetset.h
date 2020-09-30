#ifndef CABINETSET_H
#define CABINETSET_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QByteArray>
#include <QStringList>
#include "cabinetconfig.h"
#include "Device/Network/qnetinterface.h"
#include <QButtonGroup>
#include <QTableWidget>
#include "Device/controldevice.h"
#include "funcs/servertest.h"

namespace Ui {
class CabinetSet;
}

class CabinetSet : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetSet(QWidget *parent = 0);
    ~CabinetSet();
    bool installGlobalConfig(CabinetConfig *globalConfig);

public slots:
    void getCardId(QByteArray id);
    void getCodeScanData(QByteArray code);
    void cloneResult(bool isSuccess, QString msg=QString());
    void regResult(bool isSuccess);

    void show();
private slots:
    void on_add_right_clicked();
    void on_clear_clicked();
    void on_save_clicked();
    void on_serverAddr_editingFinished();
    void on_lock_test_clicked();
    void on_lock_group_clicked(int id);
    void on_pushButton_clicked();
    void on_netUpdate_clicked();
    void on_netSet_clicked();
    void on_devState_toggled(bool checked);
    void on_cloneStart_clicked();
    void on_regId_clicked();
    void on_savePos_clicked();
    void on_finish_clicked();
    void on_cabType_currentIndexChanged(int index);
    void on_tabExp_clicked(const QModelIndex &index);
    void resetRegState();
    void on_aio_mode_toggled(bool checked);
    void on_apiProName_activated(const QString &arg1);

    void on_high_val_mode_toggled(bool checked);

    void on_rfid_mode_toggled(bool checked);

signals:
    void winSwitch(int index);//窗口切换
    //设置柜子组合:QByteArray
    //QByteArray为柜子组合信息数组，例：QByteArray qba,qba为保存此信息的数组。
    //qba[x]=y 表示第x个柜子，柜子序号是0,1,2...连续递增。y是柜子位置信息，1,3,5...单数编号表示主柜左边的位置，双数编号表示主柜右边的位置。
    void setCabinet(QByteArray);
    void cabinetCreated();
    void updateServerAddr();
    void lockTest();
    void requireOpenCase(int,int);
    void cabinetClone(QString id);
    void requireCabRigster();

private:
    Ui::CabinetSet *ui;
    int initStep;
    QStringList cabTypeList;
    ServerTest* sTest;
    QNetInterface* dev_network;
    CabinetConfig* config;
    QByteArray cabinet_pos;
    QList<QTableWidget*> list_cabinet;
    QButtonGroup group_lock;
    QStringList list_layout;
    QPoint screenPos;
    bool needSelScreen;

    inline void checkDevice();
    void initCabType();
    void cabSplit(QString scale, QTableWidget* table);
    int getBaseCount(QString scale);
    void warningSelScreen(bool waringOn);
    void layoutInit();
    void setCabType();
};

#endif // CABINETSET_H

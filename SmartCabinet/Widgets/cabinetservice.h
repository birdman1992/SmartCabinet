#ifndef CABINETSERVICE_H
#define CABINETSERVICE_H

#include <QWidget>
#include <QNetworkInterface>
#include <QShowEvent>
#include <qlist.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <QTimer>
#include <QButtonGroup>
#include "Device/Network/qnetinterface.h"
#include "cabinetconfig.h"
#include "Widgets/cabinetctrlconfig.h"

namespace Ui {
class CabinetService;
}

class CabinetService : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetService(QWidget *parent = 0);
    ~CabinetService();

    bool installGlobalConfig(CabinetConfig *globalConfig);
signals:
    void winSwitch(int);
    void requireOpenLock(int seqNum, int lockId);
    void requireClear();

private slots:
    void on_back_clicked();
    void on_addr_returnPressed();
//    void on_addr_textEdited(const QString &arg1);
    void on_ok_clicked();
    void on_cancel_clicked();
    void ctrl_lock(int);
    void initNetwork();
    void on_clear_clicked();
    void on_init_clicked();
    void on_check_clicked();
    void ctrl_conf(int id);
    void updateBtn();

public slots:
    void ctrl_boardcast();//广播控制

private:
    Ui::CabinetService *ui;
    QNetInterface* dev_network;
    QString dev_ip;
    QString dev_netmask;
    QString dev_gateway;
    QList<QCheckBox*> l_board_num;//锁控板号
    QButtonGroup l_lock_num;//锁号
    CabinetConfig* config;
    QButtonGroup l_lock_conf;
    CabinetCtrlConfig* win_ctrl_config;
    int curId;

    void updateNetInfo();
    void initStack();
    void initGroup();
    void creatCtrlConfig();

    void showEvent(QShowEvent*);
    bool eventFilter(QObject *w, QEvent *e);
    void paintEvent(QPaintEvent *event);

};

#endif // CABINETSERVICE_H

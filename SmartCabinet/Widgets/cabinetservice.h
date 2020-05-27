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
#include <QLayout>
#include "Device/Network/qnetinterface.h"
#include "cabinetconfig.h"
#include "Widgets/cabinetctrlconfig.h"
#include "manager/cabinetmanager.h"
#include "funcs/servertest.h"
#include "FingerPrint/fingerprint.h"

bool posSort(Cabinet *A, Cabinet *B);

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
    void requireInsertCol(int, int);
    void requireInsertCol(int pos, QString layout);
    void requireInsertUndo();
    void winSwitch(int);
    void requireOpenLock(int seqNum, int lockId);
    void requireClear();
    void requireUpdateServerAddress();
    void checkVersion(bool needConfirm);
    void updateStart();
    void doorState(int);
    void userCardActive(QByteArray cardId);

private slots:
    void on_back_clicked();
    void on_addr_returnPressed();
    void on_insert_num_2_valueChanged(int arg1);
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
    void on_rebind_clicked();
    void on_set_server_addr_clicked();
    void on_server_addr_editingFinished();
    void on_insert_clicked();
    void on_insert_pos_valueChanged(int arg1);
    void on_insert_pos_2_valueChanged(int arg1);
    void on_col_layout_activated(const QString &arg1);
    void on_undo_clicked();
    void on_insert_2_clicked();
    void updateNetInfo();
    void on_testServer_clicked();
    void on_set_spec_case_clicked();
    void on_scan_list_toggled(bool checked);
    void on_scan_all_toggled(bool checked);
//    void on_proName_currentTextChanged(const QString &arg1);
    void on_proName_activated(const QString &arg1);

public slots:
    void ctrl_boardcast();//广播控制
    void recvInsertColResult(bool success);
    void recvInsertUndoResult(bool success);
    void tsCalibration();
    void recvVersionInfo(bool needUpdate, QString version);
    void recvCurCardId(QByteArray cardId);

private:
    Ui::CabinetService *ui;
    QNetInterface* dev_network;
    CabinetManager* cabManager;
    QTableWidget* nTab;
    ServerTest* sTest;
    FingerPrint* win_fingerPrint;
    QString insert_layout;
    int insert_pos;
    QString dev_ip;
    QString dev_netmask;
    QString dev_gateway;
    QString dev_mac;
    QList<QCheckBox*> l_board_num;//锁控板号
    QButtonGroup l_lock_num;//锁号
    CabinetConfig* config;
    QButtonGroup l_lock_conf;
    CabinetCtrlConfig* win_ctrl_config;
    QHBoxLayout* cfg_layout;
    QList<QTableWidget*> list_preview;
    int curId;
    bool lockConfigIsOk;
    bool updateOk;

    void initStack();
    void initGroup();
    void creatCtrlConfig();
    void showVerInfo();
    bool inserCol(int pos, int num);
    bool inserCol(int pos, QString layout);
    void saveInsert();

    void showEvent(QShowEvent*);
    bool eventFilter(QObject *w, QEvent *e);
    void paintEvent(QPaintEvent *event);

    void cabSplit(QString scale, QTableWidget *table);
    QList<QTableWidget*> creatPreviewList(QStringList layouts);
    int getBaseCount(QString scale);
    void updateCabpreview(QTableWidget *newTab, int pos);
    void updateCabpreviewScr();
};

#endif // CABINETSERVICE_H

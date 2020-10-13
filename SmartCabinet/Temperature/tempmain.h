#ifndef TEMPMAIN_H
#define TEMPMAIN_H

#include <QWidget>
#include "tempcase.h"
#include <QDateTime>
#include <QTcpServer>
#include <QList>
#include <QMap>
#include <QStringList>
#include <QColor>
#include <QRgb>
#include <QBrush>
#include "http/httpserver.h"
#include "userinfo.h"
#include "Temperature/tempmanager.h"
#include "cabinetconfig.h"

class HttpServer;

namespace Ui {
class TempMain;
}

class TempMain : public QWidget
{
    Q_OBJECT

public:
    explicit TempMain(QWidget *parent = 0);
    ~TempMain();

public slots:
//    void updateCabInfo(QString id, QString name);
    void updateLoginUser(UserInfo* user);
    void logOut();
    void updateMsg(QString);
    void updateScanData(QString scanData);
    void updateTime();

    TempCase* getDevCase(QString devId);

signals:
    void reqSetting();

private:
    Ui::TempMain *ui;
    QStringList weekList;
    HttpServer* httpServer;
    QTcpServer* server;
    UserInfo* curUser;
    TempCase* cur_case;
    QList<TempCase*> list_case;
    QMap<QString, TempCase*> map_case;//ip&id-TempCase
    TempManager* tempManager;
    CabinetConfig* cabConfig;
    int fWatchdog;

    void paintHistory();
    void paintHistory(TempCase* pCase);
    void paintDataRange(int max, int min);
    void paintYAxis();
    void releaseHistory();
    void frameCaseShow(TempCase*);
    void updateCaseLayout();

    void watchdogStart();
private slots:
    void newConnection();
    void updateDevInfo(QString dev);
//    void deviceIdUpdate(TempCase*);
    void on_set_clicked();
    void on_history_toggled(bool checked);
    void caseClicked(TempCase*);
    void caseIdUpdate(TempCase*);
    int watchdogTimeout();
    void on_history_clicked(bool checked);
    void on_showStack_currentChanged(int arg1);
    void on_save_params_clicked();
};

#endif // TEMPMAIN_H

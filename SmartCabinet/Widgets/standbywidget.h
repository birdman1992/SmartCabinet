#ifndef STANDBYWIDGET_H
#define STANDBYWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include "cabinetconfig.h"

namespace Ui {
class StandbyWidget;
}

class StandbyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StandbyWidget(QWidget *parent = 0);
    ~StandbyWidget();

    bool installGlobalConfig(CabinetConfig *globalConfig);

public slots:
    void recvUserInfo(QByteArray qba);
private slots:
    void wait_timeout();
    void on_cun_clicked();

signals:
    void winSwitch(int);

private:
    Ui::StandbyWidget *ui;
    bool waitForCardReader;
    QMessageBox* msgBox;
    CabinetConfig* config;

    void warningMsgBox(QString title, QString msg);
};

#endif // STANDBYWIDGET_H

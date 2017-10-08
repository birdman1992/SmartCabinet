#ifndef CABINETCTRLCONFIG_H
#define CABINETCTRLCONFIG_H

#include <QWidget>
#include "cabinetconfig.h"

namespace Ui {
class CabinetCtrlConfig;
}

class CabinetCtrlConfig : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetCtrlConfig(QWidget *parent = 0);
    void configStart(int seq, int index);
    ~CabinetCtrlConfig();

    bool installGlobalConfig(CabinetConfig *globalConfig);
private slots:
    void on_clear_clicked();

    void on_test_clicked();

    void on_ok_clicked();

    void on_cancel_clicked();

signals:
    void lockCtrl(int seq, int index);
    void updateBtn();

private:
    Ui::CabinetCtrlConfig *ui;
    CabinetConfig* config;
    int curSeq;
    int curIndex;

    void paintEvent(QPaintEvent *);
};

#endif // CABINETCTRLCONFIG_H

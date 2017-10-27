#ifndef CABINETREFUND_H
#define CABINETREFUND_H

#include <QWidget>
#include <QPaintEvent>
#include "Structs/caseaddress.h"
#include "Structs/cabinetinfo.h"
#include "cabinetconfig.h"
#include <Widgets/cabinetrefunditem.h>
#include <QList>

namespace Ui {
class cabinetRefund;
}

class CabinetRefund : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetRefund(QWidget *parent = 0);
    ~CabinetRefund();
    void refundStart(CaseAddress addr);
    bool installGlobalConfig(CabinetConfig *globalConfig);
    void refundRst(QString msg);
    void refundScan(QString scanId, QString fullId);

private slots:
    void on_pushButton_clicked();
    void on_ok_clicked();

signals:
    void refundCase(QStringList, int);

private:
    Ui::cabinetRefund *ui;
    CaseAddress curAddr;
    CabinetInfo* curRefundCab;
    CabinetConfig* config;
    QList<CabinetRefundItem*> list_item;
    QStringList list_code;

    void paintEvent(QPaintEvent *);
    void tableClear();
    bool codeAppend(QString code);
    void codeAppendCancel();

};

#endif // CABINETREFUND_H

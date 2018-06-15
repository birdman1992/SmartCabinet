#ifndef CABINETCHECK_H
#define CABINETCHECK_H

#include <QWidget>
#include <QPaintEvent>
#include "Structs/caseaddress.h"
#include "Structs/cabinetinfo.h"
#include "cabinetconfig.h"
#include <Widgets/cabinetcheckitem.h>
#include <QList>

namespace Ui {
class CabinetCheck;
}

class CabinetCheck : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetCheck(QWidget *parent = 0);
    ~CabinetCheck();
    void checkStart(CaseAddress addr);
    bool installGlobalConfig(CabinetConfig *globalConfig);
    void checkStop();
    void checkRst(QString msg);
    void checkScan(QString scanId, QString fullId);

public slots:
    void show();
private slots:
    void on_pushButton_clicked();
    void on_ok_clicked();

signals:
    void checkCase(QList<CabinetCheckItem*>,CaseAddress);
    void checkCase(QStringList, CaseAddress);

private:
    Ui::CabinetCheck *ui;
    CaseAddress curAddr;
    CabinetInfo* curCheckCab;
    CabinetConfig* config;
    QList<CabinetCheckItem*> list_item;
    QStringList list_code;

    void paintEvent(QPaintEvent *);
    void tableClear();
    bool codeAppend(QString code);
    void codeAppendCancel();

};

#endif // CABINETCHECK_H

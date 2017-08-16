#ifndef CABINETSET_H
#define CABINETSET_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QByteArray>
#include"cabinetconfig.h"

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

private slots:
    void on_add_left_clicked();
    void on_add_right_clicked();
    void on_clear_clicked();
    void on_save_clicked();

    void on_serverAddr_editingFinished();

    void on_lock_test_clicked();

signals:
    void winSwitch(int index);//窗口切换
    //设置柜子组合:QByteArray
    //QByteArray为柜子组合信息数组，例：QByteArray qba,qba为保存此信息的数组。
    //qba[x]=y 表示第x个柜子，柜子序号是0,1,2...连续递增。y是柜子位置信息，1,3,5...单数编号表示主柜左边的位置，双数编号表示主柜右边的位置。
    void setCabinet(QByteArray);
    void cabinetCreated();
    void updateServerAddr(QString addr);
    void lockTest();

private:
    Ui::CabinetSet *ui;
    CabinetConfig* config;
    QByteArray cabinet_pos;
    QList<QLabel*> list_cabinet;
};

#endif // CABINETSET_H

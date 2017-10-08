#ifndef USERWIDGET_H
#define USERWIDGET_H

#include <QWidget>
#include <QShowEvent>
#include <QMessageBox>
#include <QByteArray>
#include "cabinetconfig.h"

namespace Ui {
class UserWidget;
}

class UserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserWidget(QWidget *parent = 0);
    bool installGlobalConfig(CabinetConfig* globalConfig);
    ~UserWidget();

private:
    Ui::UserWidget *ui;
    CabinetConfig* config;
    QMessageBox* msgBox;
    bool waitForCardReader;//等待读卡器数据
    bool firstUse;//第一次使用

    void listUpdate();
    bool configCheck();
    void showEvent(QShowEvent* event);

public slots:
    void recvUserInfo(QByteArray);//接收读卡数据
    void updateUserList();//刷新用户列表

signals:
    void winSwitch(int index);//窗口切换

private slots:
    void on_addUser_clicked();
    void on_addOk_clicked();
    void on_addUser_2_clicked();
    void on_addUser_3_clicked();
    void on_addUser_4_clicked();
};

#endif // USERWIDGET_H

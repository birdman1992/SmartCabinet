#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "Cabinet/cabinetpanel.h"
#include <QPushButton>
#include <QSettings>
#include "Device/controldevice.h"
#include "medinf.h"
#include <QList>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include "Menu/setmenu.h"
#include "ShowInf/showinf.h"
#include "PrimaryUser/primaryuser.h"
#include "PrimaryUser/userinf.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void btn_one();
    void btn_two();
    void btn_three();
    void btn_four();
    void check_code(QByteArray);
    void read_showinf(MedInf );
    void New_Pri_User(UserInf);
    void scan_user(QByteArray);

protected:
    int readSettings();                //--读配置信息
    void writeSettings();              //--写配置信息
    void menu_set_init();
    void check_pri_use();

private:
    Ui::MainWidget *ui;

    CabinetPanel cabinets[5];          //--定义5药柜

    int num;                           //--药柜数目
    int lattice_num;                   //--药柜格子数目
    int cab_lattice_num[5];            //--主药柜格子数目

    QString path_cabinet;              //药柜文件路径

    QList<MedInf> medinf[5];           //--定义5个qlist，用于存储药柜信息
    QList<UserInf> USER;

    ControlDevice *ctrlUi;//新添加的控制类，只需要和3个信号对接，详情见signals

    QStackedWidget *stack;             //堆栈窗体
    QListWidget *list;                 //列表框
    QWidget *cab_widget;
    SetMenu *menu_widget;
    ShowInf *show_inf;
    PrimaryUser *Pri_user;

    QVBoxLayout *qvbox_menu_layout;

    QHBoxLayout *qhbox_main;
    QVBoxLayout *qvbox_zero_layout;
    QVBoxLayout *qvbox_one_layout;
    QVBoxLayout *qvbox_two_layout;
    QVBoxLayout *qvbox_three_layout;
    QVBoxLayout *qvbox_four_layout;

    QPushButton *btn_cabinet_add_one;   //--添加药柜按钮
    QPushButton *btn_cabinet_add_two;
    QPushButton *btn_cabinet_add_three;
    QPushButton *btn_cabinet_add_four;

    QLabel *label;

};

#endif // MAINWIDGET_H

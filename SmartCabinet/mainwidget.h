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
    void cabinets_lattice_zero(int row);
    void cabinets_lattice_one(int row);
    void cabinets_lattice_two(int row);
    void cabinets_lattice_three(int row);
    void cabinets_lattice_four(int row);

protected:
    void mkdir_cabinet();
    void mkdir_cabinet_txt(QString name,CabinetPanel *cab);
    int readSettings();                //--读配置信息
    void writeSettings();              //--写配置信息
    void menu_set_init();
    void lattice_add(int cab, int row);//--添加药柜格子

private:
    Ui::MainWidget *ui;
//<<<<<<< HEAD

    CabinetPanel cabinets[5];          //--定义5药柜

    int num;                           //--药柜数目
    int lattice_num;                   //--药柜格子数目
    int cab_lattice_num[5];            //--主药柜格子数目

    QString path_cabinet;              //药柜文件路径

    QList<MedInf> medinf[5];           //--定义5个qlist，用于存储药柜信息
//=======
    ControlDevice ctrlUi;//新添加的控制类，只需要和3个信号对接，详情见signals
//    CabinetPanel cabinets[5];//--定义5药柜
    QPushButton *btn_left; //--添加药柜按钮
    QPushButton *btn_add_lattice;
    QPushButton *btn_right;
//>>>>>>> birdman1992/master

    QStackedWidget *stack;             //堆栈窗体
    QListWidget *list;                 //列表框
    QWidget *cab_widget;
    QWidget *menu_widget;
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

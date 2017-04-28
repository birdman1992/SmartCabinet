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
    void btn_cabinet_add();//--添加药柜
    void btn_lattice_add();//--添加药柜格子

protected:
    void mkdir_cabinet();
    void mkdir_cabinet_txt(QString name,CabinetPanel *cab);
    void readSettings();//--读配置信息
    void writeSettings();//--写配置信息

private:
    Ui::MainWidget *ui;
    ControlDevice ctrlUi;//新添加的控制类，只需要和3个信号对接，详情见signals
    CabinetPanel cabinets[5];//--定义5药柜
    QPushButton *btn_left; //--添加药柜按钮
    QPushButton *btn_add_lattice;
    QPushButton *btn_right;

    int num;//--药柜数目
    int lattice_num;//--药柜格子数目
    int cab_lattice_num;//--主药柜格子数目

    QString path_cabinet;//药柜文件路径

    QList<MedInf> medinf[5];//--定义5个qlist，用于存储药柜信息

    QVBoxLayout *btn_box;

};

#endif // MAINWIDGET_H

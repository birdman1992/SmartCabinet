#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "Cabinet/cabinetpanel.h"
#include <QPushButton>
#include <QSettings>
#include "Device/controldevice.h"

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
    void btn_cabinet_add();

protected:
    void mkdir_cabinet();
    void mkdir_cabinet_txt(QString name,CabinetPanel *cab);
    void readSettings();
    void writeSettings();

private:
    Ui::MainWidget *ui;
    ControlDevice ctrlUi;
    CabinetPanel* cabinets;//--定义药柜
    CabinetPanel* cabinets1;
    CabinetPanel* cabinets2;
    CabinetPanel* cabinets3;
    CabinetPanel* cabinets4;
    QPushButton *btn_left; //--添加药柜按钮
    QPushButton *btn_right;
    int num;//--药柜数目
    QString path_cabinet;//药柜文件路径

};

#endif // MAINWIDGET_H

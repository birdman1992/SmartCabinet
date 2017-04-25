#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "Cabinet/cabinetpanel.h"
#include <QPushButton>
#include <QSettings>

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
    CabinetPanel* cabinets;
    CabinetPanel* cabinets1;
    CabinetPanel* cabinets2;
    CabinetPanel* cabinets3;
    CabinetPanel* cabinets4;
    QPushButton *btn_left;
    QPushButton *btn_right;
    int num;
    QString path_cabinet;//药柜文件路径

};

#endif // MAINWIDGET_H

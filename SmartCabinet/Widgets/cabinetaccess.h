#ifndef CABINETACCESS_H
#define CABINETACCESS_H

#include <QWidget>

namespace Ui {
class CabinetAccess;
}

class CabinetAccess : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetAccess(QWidget *parent = 0);
    ~CabinetAccess();
//    accessInit(bool store, );
    void setAccessModel(bool store);//true:存 false:取

private:
    Ui::CabinetAccess *ui;
    bool isStore;
};

#endif // CABINETACCESS_H

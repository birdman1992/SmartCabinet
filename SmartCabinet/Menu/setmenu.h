#ifndef SETMENU_H
#define SETMENU_H

#include <QWidget>
#include "Countdown/countdown.h"
#include "PrimaryUser/primaryuser.h"
namespace Ui {
class SetMenu;
}

class SetMenu : public QWidget
{
    Q_OBJECT

public:
    explicit SetMenu(QWidget *parent = 0);
    ~SetMenu();
    void count_close();
private slots:
    void btn_in();
    void btn_add();

private:
    Ui::SetMenu *ui;
    CountDown *count_down;
    PrimaryUser *user;
};

#endif // SETMENU_H

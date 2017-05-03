#ifndef SETMENU_H
#define SETMENU_H

#include <QWidget>
#include "Countdown/countdown.h"

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

private:
    Ui::SetMenu *ui;
    CountDown *count_down;
};

#endif // SETMENU_H

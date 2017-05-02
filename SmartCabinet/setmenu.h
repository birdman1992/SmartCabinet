#ifndef SETMENU_H
#define SETMENU_H

#include <QWidget>

namespace Ui {
class SetMenu;
}

class SetMenu : public QWidget
{
    Q_OBJECT

public:
    explicit SetMenu(QWidget *parent = 0);
    ~SetMenu();

private:
    Ui::SetMenu *ui;
};

#endif // SETMENU_H

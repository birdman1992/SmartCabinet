#ifndef PRIMARYUSER_H
#define PRIMARYUSER_H

#include <QWidget>
#include "userinf.h"
#include "Device/controldevice.h"

namespace Ui {
class PrimaryUser;
}

class PrimaryUser : public QWidget
{
    Q_OBJECT

public:
    explicit PrimaryUser(QWidget *parent = 0);
    ~PrimaryUser();
signals:
    void new_pri_user(UserInf);

public slots:
    void btn_sure();
    void btn_quit();
    void scan_user_inf(QByteArray);

private:
    Ui::PrimaryUser *ui;
};

#endif // PRIMARYUSER_H

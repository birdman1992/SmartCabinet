#ifndef SHOWINF_H
#define SHOWINF_H

#include <QWidget>
#include "medinf.h"

namespace Ui {
class ShowInf;
}

class ShowInf : public QWidget
{
    Q_OBJECT

public:
    explicit ShowInf(QWidget *parent = 0);
    ~ShowInf();
    void check_exist(MedInf med);
    void check_no_exist(MedInf med);
public slots:
    void btn_save();
    void btn_close();
signals:
    void cabinet_inf(int row,int low,int num);
    void inf_save();
private:
    Ui::ShowInf *ui;
    int num;
    int add_num;
    int cabinet_num;
    int lattice_num;
};

#endif // SHOWINF_H

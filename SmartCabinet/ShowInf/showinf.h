#ifndef SHOWINF_H
#define SHOWINF_H

#include <QWidget>

namespace Ui {
class ShowInf;
}

class ShowInf : public QWidget
{
    Q_OBJECT

public:
    explicit ShowInf(QWidget *parent = 0);
    ~ShowInf();

private:
    Ui::ShowInf *ui;
};

#endif // SHOWINF_H

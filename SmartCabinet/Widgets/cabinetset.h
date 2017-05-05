#ifndef CABINETSET_H
#define CABINETSET_H

#include <QWidget>
#include <QList>
#include <QLabel>
#include <QByteArray>

namespace Ui {
class CabinetSet;
}

class CabinetSet : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetSet(QWidget *parent = 0);
    ~CabinetSet();

private slots:
    void on_add_left_clicked();
    void on_add_right_clicked();
    void on_clear_clicked();
    void on_save_clicked();
    void on_cancel_clicked();

signals:
    void winSwitch(int index);//窗口切换
    void setCabinet(QByteArray);//设置柜子组合

private:
    Ui::CabinetSet *ui;
    QByteArray cabinet_pos;
    QList<QLabel*> list_cabinet;
};

#endif // CABINETSET_H

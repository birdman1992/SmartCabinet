#ifndef OPERATION_H
#define OPERATION_H

#include <QWidget>

namespace Ui {
class Operation;
}

class Operation : public QWidget
{
    Q_OBJECT

public:
    explicit Operation(QWidget *parent = nullptr);
    ~Operation();
    QString curOperation();//当前手术单

public slots:
    void loadOperations();//加载手术单

signals:
    void requireUpdate();
    void updateSelOperation(QString info);//刷新当前选择手术单信息
    void winClose();

private slots:
    void on_back_clicked();//返回
    void on_update_clicked();//刷新

private:
    Ui::Operation *ui;

};

#endif // OPERATION_H

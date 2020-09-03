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

private:
    Ui::Operation *ui;
};

#endif // OPERATION_H

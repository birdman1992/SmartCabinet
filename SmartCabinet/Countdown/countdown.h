#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <QWidget>

namespace Ui {
class CountDown;
}

class CountDown : public QWidget
{
    Q_OBJECT

public:
    explicit CountDown(QWidget *parent = 0);
    ~CountDown();

private:
    Ui::CountDown *ui;
};

#endif // COUNTDOWN_H

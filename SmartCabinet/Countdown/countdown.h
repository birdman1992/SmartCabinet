#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include <QWidget>
#include <QTimer>
namespace Ui {
class CountDown;
}

class CountDown : public QWidget
{
    Q_OBJECT

public:
    explicit CountDown(QWidget *parent = 0);
    ~CountDown();

    int i;
    QTimer *timer;
public slots:
    void time_out();
private:
    Ui::CountDown *ui;
};

#endif // COUNTDOWN_H

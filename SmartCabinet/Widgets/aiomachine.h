#ifndef AIOMACHINE_H
#define AIOMACHINE_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include "aiobutton.h"

namespace Ui {
class AIOMachine;
}

class AIOMachine : public QWidget
{
    Q_OBJECT

public:
    explicit AIOMachine(QWidget *parent = 0);
    ~AIOMachine();

private:
    Ui::AIOMachine *ui;
    void paintEvent(QPaintEvent *);
};

#endif // AIOMACHINE_H

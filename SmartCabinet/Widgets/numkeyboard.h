#ifndef NUMKEYBOARD_H
#define NUMKEYBOARD_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QButtonGroup>
#include "inputcontex/keyboard.h"

namespace Ui {
class NumKeyboard;
}

class NumKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit NumKeyboard(QWidget *parent = 0);
    ~NumKeyboard();

signals:
    void key(int num);
    void backspace();
    void clearAll();

private:
    Ui::NumKeyboard *ui;
    void paintEvent(QPaintEvent *);
    QButtonGroup group_key;

private slots:
    void onKeysClicked(int);
};

#endif // NUMKEYBOARD_H

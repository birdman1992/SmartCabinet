#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QPainter>
#include <QTimer>

namespace Ui {
class KeyBoard;
}

class KeyBoard : public QWidget
{
    Q_OBJECT

public:
    explicit KeyBoard(QWidget *parent = 0);
    ~KeyBoard();

signals:
    void key(uint);
    void keyPress(uint);
    void keyRelease(uint);

private slots:
    void onKeysClicked(int val);
    void saveFocusWidget(QWidget *, QWidget *newFocus);
    void onKeysPressed(int val);
    void onKeysReleased(int val);
    void on_key_back_pressed();
    void on_key_back_released();
    void backspace_timeout();

private:
    Ui::KeyBoard *ui;
    QWidget *lastFocusedWidget;
    QTimer* timer_backspace;
    void paintEvent(QPaintEvent *);
    QButtonGroup group_key;
    bool event(QEvent *e);
};

#endif // KEYBOARD_H

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QWidget>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QPainter>

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
    void key(ushort);

private slots:
    void onKeysClicked(int val);
    void saveFocusWidget(QWidget *, QWidget *newFocus);
private:
    Ui::KeyBoard *ui;
    QWidget *lastFocusedWidget;
    void paintEvent(QPaintEvent *);
    QButtonGroup group_key;
    bool event(QEvent *e);
};

#endif // KEYBOARD_H

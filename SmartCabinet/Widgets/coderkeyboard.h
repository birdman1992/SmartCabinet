#ifndef CODERKEYBOARD_H
#define CODERKEYBOARD_H

#include <QWidget>
#include <QByteArray>
#include <QPainter>
#include <QPaintEvent>
#include <QButtonGroup>
#include <qstring.h>

namespace Ui {
class coderKeyboard;
}

class coderKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit coderKeyboard(QWidget *parent = 0);
    ~coderKeyboard();

private:
    Ui::coderKeyboard *ui;
    QButtonGroup group_key;
    void paintEvent(QPaintEvent *);
    QString str;

signals:
    void coderData(QByteArray);
private slots:
    void onKeysClicked(int val);
};

#endif // CODERKEYBOARD_H

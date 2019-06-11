#ifndef AIOBUTTON_H
#define AIOBUTTON_H

#include <QObject>
#include <QPushButton>
#include <QPaintEvent>
#include <QPainter>

class AIOButton : public QPushButton
{
    Q_OBJECT
public:
    AIOButton(QWidget* parent=NULL);

private:
    void paintEvent(QPaintEvent*);
};

#endif // AIOBUTTON_H

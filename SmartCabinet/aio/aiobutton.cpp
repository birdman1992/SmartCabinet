#include "aiobutton.h"

AIOButton::AIOButton(QWidget *parent):
    QPushButton(parent)
{

}

void AIOButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawText(this->geometry(),Qt::AlignLeft | Qt::AlignBottom , this->text());
}

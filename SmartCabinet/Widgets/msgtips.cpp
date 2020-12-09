#include "msgtips.h"
#include <QFontMetrics>


MsgTips::MsgTips(QWidget *parent):
    QLabel(parent)
{
    this->setWordWrap(false);
    this->setWindowFlags(Qt::FramelessWindowHint);
    font = new QFont("WenQuanYi Micro Hei Mono");
    font->setPixelSize(28);
    this->setFont(*font);
    this->setStyleSheet("{color:#fff;backgrount-color:#1D9969;}");
}

MsgTips::~MsgTips()
{
    delete font;
}

void MsgTips::showText(QString text)
{
    QFontMetrics textMet(*font);
    QRect textRect = textMet.boundingRect(text);
    qDebug()<<"text rect"<<textRect;
    this->resize(textRect.size());
    this->setText(text);
    this->move(0,0);
    this->show();
}

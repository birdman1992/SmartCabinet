#include "msgtips.h"
#include <QFontMetrics>
#include <QTime>
#include <QtDebug>
#include <QPoint>
#include <QApplication>
#include <QDesktopWidget>

MsgTips::MsgTips(QWidget *parent):
    QLabel(parent)
{
    this->setWordWrap(false);
    this->setWindowFlags(Qt::FramelessWindowHint);
    font = new QFont("WenQuanYi Micro Hei Mono");
    font->setPixelSize(28);
    this->setFont(*font);
    this->setAlignment(Qt::AlignVCenter|Qt::AlignLeft);
    this->setStyleSheet("color:#fff;"
                        "background-color:#1D9969;"
                        "border:1px solid #14F59E;");
    this->show();
    this->lower();
    qDebug()<<"[MsgTips]";
}

MsgTips::~MsgTips()
{
    delete font;
}

/**
 * @brief MsgTips::showText
 * @param anchor 信息框锚定对象
 * @param text  显示文本
 */
void MsgTips::showText(QWidget* anchor, QStringList texts)
{
//    QTime timerShow;
//    timerShow.start();
    QRect textRect = calTextRect(anchor, texts);
    this->setGeometry(textRect);
    this->setText(texts.join("\n"));
    this->raise();
}

QRect MsgTips::calTextRect(QWidget* anchor, QStringList texts)
{
    QString maxWidthText;
    foreach (QString text, texts)
    {
        if(text.length()>maxWidthText.length())
            maxWidthText = text;
    }

    QFontMetrics textMet(*font);
    QRect textRect = textMet.boundingRect(maxWidthText);
    textRect.setHeight((textMet.height()+4)*texts.count()+20);
    textRect.setWidth(textRect.width()+20);

    QRect anchorRect = anchor->geometry();
    anchorRect.moveTo(anchor->mapToGlobal(QPoint(0, 0)));//左上角转换为全局坐标

    QRect desktopRect = QApplication::desktop()->geometry();

    bool anchorRight = ((anchorRect.right()+textRect.width()) <= desktopRect.right());
    bool anchorTop = ((anchorRect.top()+textRect.height()) <= desktopRect.bottom());

    QPoint anchorPos;
    if(anchorRight)
        anchorPos.setX(anchorRect.right());
    else
        anchorPos.setX(anchorRect.left()-textRect.width());

    if(anchorTop)
        anchorPos.setY(anchorRect.top());
    else
        anchorPos.setY(desktopRect.bottom() - textRect.height());

    qDebug()<<"anchor pos:"<<anchorPos;
    qDebug()<<"anchor text"<<textRect;
    qDebug()<<"anchor rect:"<<anchorRect;
    qDebug()<<"anchorRight:"<<anchorRight<<"anchorTop:"<<anchorTop;
    textRect.moveTo(anchorPos);

    return textRect;
}

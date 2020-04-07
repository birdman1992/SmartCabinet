#include "iconhelper.h"

IconHelper* IconHelper::_instance = 0;
IconHelper::IconHelper(QObject *) :
    QObject(qApp)
{
    int fontId = QFontDatabase::addApplicationFont(":/iconfont/fontawesome-webfont.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    iconFont = QFont(fontName);
}

QFont IconHelper::loadFont(QString fontSrc)
{
    int fontId = QFontDatabase::addApplicationFont(":/iconfont/fontawesome-webfont.ttf");
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    iconFont = QFont(fontName);
}

void IconHelper::SetIcon(QLabel *lab, QChar c, int size)
{
    iconFont.setPixelSize(size);
    lab->setFont(iconFont);
    lab->setText(c);
}

void IconHelper::SetIcon(QPushButton *btn, QChar c, int size)
{
    iconFont.setPixelSize(size);
    btn->setFont(iconFont);
    btn->setText(c);
}

void IconHelper::SetIcon(QPushButton *btn, QString c, int size)
{
    iconFont.setPixelSize(size);
    btn->setFont(iconFont);
    btn->setText(c);
}

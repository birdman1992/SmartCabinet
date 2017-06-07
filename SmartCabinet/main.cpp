#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QFont>
#include "beautifului.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    beautiful::SetUTF8Code();//--显示中文 utf-8
//    beautiful::SetStyle("image/black");//黑色风格
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
    QFont font;
    font.setFamily(("msyh"));
    a.setFont(font);
    MainWidget w;
    w.show();

    return a.exec();
}

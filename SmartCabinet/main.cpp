#include "mainwidget.h"
#include <QApplication>
#include<QTextCodec>
#include "beautifului.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    beautiful::SetUTF8Code();//--显示中文 utf-8
    beautiful::SetStyle("image/black");//黑色风格

    MainWidget w;
    w.show();

    return a.exec();
}

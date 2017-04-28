#include "mainwidget.h"
#include <QApplication>
#include<QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //--显示中文
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF8"));
    MainWidget w;
    w.show();

    return a.exec();
}

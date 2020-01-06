#include "frmrfid.h"
#include <QApplication>
#include <QMetaType>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<EpcInfo*>("EpcInfo*");
    qRegisterMetaType<TableMark>("TableMark");
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    FrmRfid w;
    w.show();

    return a.exec();
}

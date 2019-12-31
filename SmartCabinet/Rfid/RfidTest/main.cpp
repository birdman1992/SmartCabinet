#include "frmrfid.h"
#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<EpcInfo*>("EpcInfo*");
    qRegisterMetaType<TableMark>("TableMark");

    FrmRfid w;
    w.show();

    return a.exec();
}

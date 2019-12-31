#include "frmrfid.h"
#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<EpcInfo*>("EpcInfo*");

    FrmRfid w;
    w.show();

    return a.exec();
}

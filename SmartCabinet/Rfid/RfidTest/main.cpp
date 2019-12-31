#include "frmrfid.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FrmRfid w;
    w.show();

    return a.exec();
}

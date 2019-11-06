#include "fingerprint.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FingerPrint w;
    w.show();

    return a.exec();
}

#include "fingerset.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FingerSet w;
    w.show();

    return a.exec();
}

#include <QCoreApplication>
#include "authormanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    AuthorManager* m = new AuthorManager;
    m->authorStart();
    return 0;
//    return a.exec();
}

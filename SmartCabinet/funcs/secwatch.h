#ifndef SECWATCH_H
#define SECWATCH_H
#include <QTime>
#include <QString>

class SecWatch {
private:
    SecWatch();
    static QTime* t;
    static SecWatch* w;
public:
    static void start();
    static void elapsed(QString tag);
    static void restart();
//    void stop();
};
#endif // SECWATCH_H

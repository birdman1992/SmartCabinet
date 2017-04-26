#ifndef QHID_H
#define QHID_H

#include <QObject>
#include <QThread>
#include <QString>

class QHid : public QThread
{
    Q_OBJECT
public:
    explicit QHid(QObject *parent = 0);
    void hidOpen(QString dev);
    void run();

private:
    int fd;
    int version;
    char name[100];

signals:
    void hidRead(QByteArray qba);
};

#endif // QHID_H

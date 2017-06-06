#ifndef QSERIALPORT_H
#define QSERIALPORT_H

#include <QObject>
#include <QThread>
#include <qstring.h>
#include <QByteArray>

class QSerialPort : public QThread
{
public:
    explicit QSerialPort(QString dev, QObject *parent = 0);
    void com_init(int baude, int c_flow, int bits, char parity, int stop);
    void com_write(QByteArray data);

private:
    int ret;
    int dev_fd;
    int open_com(char *DevPath);
    int uart_close(int fd);
    int uart_set(int fd, int baude, int c_flow, int bits, char parity, int stop);
    ssize_t safe_write(int fd, const void *vptr, size_t n);
    ssize_t safe_read(int fd, void *vptr, size_t n);
    int uart_read(int fd, char *r_buf, size_t len);
    int uart_write(int fd, const char *w_buf, size_t len);
    int open_com2(char *DevPath);
};

#endif // QSERIALPORT_H

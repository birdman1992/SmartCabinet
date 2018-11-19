#ifndef AUTHORENCRYPT_H
#define AUTHORENCRYPT_H

#include <QObject>
#include <QCryptographicHash>
#include <QByteArray>

class AuthorEncrypt : public QObject
{
    Q_OBJECT
public:
    explicit AuthorEncrypt(QObject *parent = 0);
    static QByteArray userEncrypt(QByteArray A);
    static QByteArray spdEncrypt(QByteArray A);

signals:

public slots:


private:
    static QByteArray encrypt(QByteArray A, QByteArray key);
};

#endif // AUTHORENCRYPT_H

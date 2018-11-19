#include "authorencrypt.h"
#define KEY_A "rmxx@m9j8JTj69*pB7rGP"
#define KEY_B "rmxx@u0t#UHRceLgAeUsS"

AuthorEncrypt::AuthorEncrypt(QObject *parent) : QObject(parent)
{

}

QByteArray AuthorEncrypt::userEncrypt(QByteArray A)
{
    return encrypt(A, KEY_A);
}

QByteArray AuthorEncrypt::spdEncrypt(QByteArray A)
{
    return encrypt(A, KEY_B);
}

QByteArray AuthorEncrypt::encrypt(QByteArray A, QByteArray key)
{
    QByteArray data = QCryptographicHash::hash(A, QCryptographicHash::Md5) + QByteArray(key);
    return QCryptographicHash::hash(data, QCryptographicHash::Md5);
}

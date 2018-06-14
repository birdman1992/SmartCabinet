#ifndef QAES_H
#define QAES_H
#include "aes.h"
#include <QByteArray>
#include <QDebug>

class QAes
{
public:
    QAes(QByteArray _key, int keyLen);
    QByteArray aes_ecb(QByteArray input);

private:
    AES_KEY key;

    QByteArray fill(QByteArray in);

};

#endif // QAES_H

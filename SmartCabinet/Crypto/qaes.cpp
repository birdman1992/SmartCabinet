#include "qaes.h"

QAes::QAes(QByteArray _key, int keyLen)
{
    AES_set_encrypt_key((unsigned char*)_key.data(), keyLen, &key);
}

QByteArray QAes::aes_ecb(QByteArray input)
{
    QByteArray ret;
    input = fill(input);
    ret.resize(input.size());
//    qDebug()<<"[aes in]"<<input.toHex();

    for(int i=0; i<(input.size()+16)/16; i++)
    {
        AES_ecb_encrypt((unsigned char*)input.data(), (unsigned char*)ret.data(), &key, AES_ENCRYPT);
    }

    return ret;
}

QByteArray QAes::fill(QByteArray in)
{
    int fillSize = 16-in.size()%16;
    if(fillSize == 0)
        fillSize = 16;
    return in+QByteArray(fillSize, fillSize);
//    QByteArray ret = QByteArray((in.size()/16+1)*16, c);
//    memcpy(ret.data(), in.data(), in.size());
//    return ret;
}

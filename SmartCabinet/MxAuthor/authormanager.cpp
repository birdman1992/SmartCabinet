#include "authormanager.h"
#define HW_OCOTP_CFG0 "0xea9affff"
#define HW_OCOTP_CFG1 "0x908ffff"
#include <QFile>
#include <QDebug>
#include <QProcess>
#include "authorencrypt.h"

AuthorManager::AuthorManager()
{

}

void AuthorManager::authorStart()
{
    QByteArray authorSrc = getAuthorSrc();
    QByteArray license = AuthorEncrypt::spdEncrypt(authorSrc);
    qDebug()<<authorSrc<<license.toBase64();
    QFile f("/home/.license");
    if(f.open(QFile::WriteOnly))
    {
        f.write(license.toBase64());
        f.close();
        QProcess::startDetached("rm /home/MxAuthor");
        return;
    }
}

bool AuthorManager::authorCheck()
{
    QByteArray src = getAuthorSrc();
    QByteArray license = AuthorEncrypt::spdEncrypt(src);

    QFile f("/home/.license");
    if(f.open(QFile::ReadOnly))
    {
        QByteArray qba = f.readAll();
        f.close();
        if(QFile("/home/MxAuthor").exists())
            QProcess::startDetached("rm /home/MxAuthor");

        if(qba == license.toBase64())
            return true;
        else
            return false;
    }
    else
        return false;
}

QByteArray AuthorManager::getAuthorSrc()
{
    QByteArray ret = QByteArray(HW_OCOTP_CFG1) + QByteArray(HW_OCOTP_CFG0);
#ifdef PC
    return ret;
#else
    QFile cfg1("/sys/fsl_otp/HW_OCOTP_CFG1");
    QFile cfg0("/sys/fsl_otp/HW_OCOTP_CFG0");
    if((!cfg1.exists()) || (!cfg0.exists()))
        return ret;

    if(!cfg1.open(QFile::ReadOnly) || !cfg0.open(QFile::ReadOnly))
        return ret;

    ret = cfg1.readAll() + cfg0.readAll();
#endif
    return ret;
}

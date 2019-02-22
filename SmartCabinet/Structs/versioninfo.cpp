#include "versioninfo.h"
#include <QDebug>
#include <QCryptographicHash>

#define EXE "SmartCabinet"

VersionInfo::VersionInfo(QString version)
{
    curVersion = version;
}

/*
{"callTime":0,
"success":true,
"errorCode":"5000",
"msg":"",
"data":{
    "File":"1.5.3.20190125_beta.tar.bz2",
    "Exe":"SmartCabinet",
    "Check":"397ea7015c5bfc537938d5531cfd938d",
    "Version":"1.5.3.20190125_beta"
    }
}
*/
bool VersionInfo::needUpdate(QByteArray qba)
{
    cJSON* json = cJSON_Parse(qba.data());
    if(json == NULL)
    {
        qDebug()<<"[needUpdate]:error json";
        return false;
    }
    cJSON* json_rst = cJSON_GetObjectItem(json, "success");
    if(json_rst->type == cJSON_True)
    {
        cJSON* json_data = cJSON_GetObjectItem(json, "data");
        if(json_data == NULL)
            return false;

        pacFile = QString(cJSON_GetObjectItem(json_data, "File")->valuestring);
        exe = QString(cJSON_GetObjectItem(json_data, "Exe")->valuestring);
        check = QString(cJSON_GetObjectItem(json_data, "Check")->valuestring);
        serverVersion = QString(cJSON_GetObjectItem(json_data, "Version")->valuestring);

        if(exe != QString(EXE))
        {
            qDebug()<<"[needUpdate]:exe inconformity";
            return false;
        }
        qDebug()<<serverVersion<<curVersion;
        if(serverVersion <= curVersion)
        {
            qDebug()<<"[needUpdate]:no new version";
            return false;
        }
        qDebug()<<"[needUpdate]:"<<serverVersion<<"update start later...";
        return true;
    }
    else
    {
        QString msg = QString(cJSON_GetObjectItem(json, "msg")->valuestring);
        qDebug()<<"[needUpdate] error:"<<msg;
        return false;
    }
}

bool VersionInfo::pacIsLegal(QFile *f)
{
    if(f == NULL)
        return false;

    if (!f->open(QFile::ReadOnly))
    {
        qDebug() << "file open error.";
        return false;
    }

    QCryptographicHash ch(QCryptographicHash::Md5);

    quint64 totalBytes = 0;
    quint64 bytesWritten = 0;
    quint64 bytesToWrite = 0;
    quint64 loadSize = 1024 * 4;
    QByteArray buf;

    totalBytes = f->size();
    bytesToWrite = totalBytes;

    while (1)
    {
        if(bytesToWrite > 0)
        {
            buf = f->read(qMin(bytesToWrite, loadSize));
            ch.addData(buf);
            bytesWritten += buf.length();
            bytesToWrite -= buf.length();
            buf.resize(0);
        }
        else
        {
            break;
        }
        if(bytesWritten == totalBytes)
        {
            break;
        }
    }

    f->close();
    QByteArray md5 = ch.result();
    md5 = QCryptographicHash::hash(md5.toHex()+serverVersion.toLocal8Bit()+QByteArray("rmxx@birdman"), QCryptographicHash::Md5);
    if(QString(md5.toHex()) == check)
        return true;
    else
    {
        qDebug()<<md5.toHex()<<check;
        return false;
    }
}


#include "temprecorder.h"
#include <QApplication>
#include <QDebug>
#include "config.h"

TempRecorder::TempRecorder(QString id)
{
    caseId = id;
    f = NULL;
    QString appPath = qApp->applicationDirPath();
    recordDir = appPath + PATH_TEMP_LOG + id + "/";
    curDate = QDate::currentDate();
    QDir dir(recordDir);
    qDebug()<<"[record dir]"<<recordDir;
    if(!dir.exists())
        if(dir.mkpath(recordDir))
        {
            qDebug("record dir mk");
        }

    f = new QFile(recordDir + recordName());
    if(!f->open(QFile::Append))
    {
        f = NULL;
        qDebug()<<recordDir + recordName()<<"open failed.";
    }
    else
    {
        qDebug()<<recordDir + recordName()<<"open success.";
    }
}

TempRecorder::~TempRecorder()
{
    if(f != NULL)
    {
        f->close();
        f->deleteLater();
    }
}

bool TempRecorder::recordTemp(float wData, float sData, int state, int area)
{
    if(isNextDay())
    {
        f->close();
        f->deleteLater();
        f = new QFile(recordDir + recordName());
        if(!f->open(QFile::Append))
        {
            f = NULL;
        }
    }
    if(f == NULL)
        return false;
    QString writeData = QString("%1 %2 %3 %4 \r\n").arg(QTime::currentTime().toString("hh:mm:ss")).arg(state).arg(wData).arg(sData);
    qDebug()<<"[recordTemp]"<<writeData;
    f->write(writeData.toLocal8Bit());
    f->flush();
    return true;
}

QString TempRecorder::recDir()
{
    return recordDir;
}

bool TempRecorder::isNextDay()
{
    if(QDate::currentDate() > curDate)
    {
        qDebug("[next day]");
        curDate = QDate::currentDate();
        return true;
    }
    else
    {
        return false;
    }
}

QString TempRecorder::recordName()
{
    return QDateTime::currentDateTime().toString("yyyyMMdd");
}

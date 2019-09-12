#include "systool.h"
#include <QApplication>
#include <QPixmap>
#include <QTime>
#include <QDesktopWidget>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace SysTool
{
    void singleShot()
    {
        QDir dir("/home/ScreenShot/");
        if(!dir.exists())
        {
            dir.mkdir("/home/ScreenShot/");
        }
        QString picName = QString("/home/ScreenShot/%1").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss.png"));
        QPixmap pic = QPixmap::grabWindow(QApplication::desktop()->winId());
        bool rst = pic.save(picName, "PNG");
        qDebug()<<"savePic"<<picName<<rst;
    }

    float getCpuTemp()
    {
        QFile f("/sys/devices/virtual/thermal/thermal_zone0/temp");
        if(!f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return 0.0;
        }

        QTextStream stream(&f);
        QString tempStr = stream.readAll();
        f.close();
        return tempStr.toFloat()/1000;
    }
}

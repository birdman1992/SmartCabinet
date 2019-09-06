#include "screenshot.h"
#include <QApplication>
#include <QPixmap>
#include <QTime>
#include <QDesktopWidget>
#include <QDebug>

namespace ScreenShot
{
    void singleShot()
    {
        QString picName = QString("/home/pics/%1").arg(QTime::currentTime().toString("hhmmss.png"));
        QPixmap pic = QPixmap::grabWindow(QApplication::desktop()->winId());
        bool rst = pic.save(picName, "PNG");
        qDebug()<<"savePic"<<picName<<rst;
    }
}

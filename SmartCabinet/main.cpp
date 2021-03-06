#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QFont>
#include <QWSServer>
#include <globalapp.h>
#include <QDateTime>
#include <QDebug>
#include "Device/QDeviceWatcher/qdevicewatcher.h"
#include "inputcontex/myinputpanelcontext.h"
#include "beautifului.h"

void customMessageHandler(QtMsgType type, const char *msg)
{
        QString txt;
        switch (type) {
        //调试信息提示
        case QtDebugMsg:
                txt = QString("Debug: %1 (%2)").arg(msg).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
                break;

        //一般的warning提示
        case QtWarningMsg:
                txt = QString("Warning: %1 (%2)").arg(msg).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        break;
        //严重错误提示
        case QtCriticalMsg:
                txt = QString("Critical: %1 (%2)").arg(msg).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
        break;
        //致命错误提示
        case QtFatalMsg:
                txt = QString("Fatal: %1 (%2)").arg(msg).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
                abort();
        }

        QFile outFile("/home/debuglog.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
}

int main(int argc, char *argv[])
{
    GlobalApp a(argc, argv);

#ifndef SIMULATE_ON
//    QWSServer::setCursorVisible(false);
#endif
    MyInputPanelContext* inputContext = new MyInputPanelContext;
    QFile outFile("/home/debuglog.txt");
    QFile lastFile("/home/lastlog.txt");

    if(lastFile.exists())
        lastFile.remove();

    if(outFile.exists())
    {
        QFile::rename("/home/debuglog.txt", "/home/lastlog.txt");
    }
    outFile.open(QIODevice::WriteOnly);
    outFile.close();

    a.setInputContext(inputContext);

//    beautiful::SetUTF8Code();//--显示中文 utf-8
//    beautiful::SetStyle("image/black");//黑色风格
#ifdef LOG_ON
    qInstallMsgHandler(customMessageHandler);
#endif
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
//    QFont font;
//    font.setFamily(("msyh"));
//    a.setFont(font);
    MainWidget w;
#ifdef SIMULATE_ON
    w.show();
#else
    w.showFullScreen();
#endif
    a.setWidows(&w);
    QFontDatabase database;

    //下面为支持简体中文字体库
    foreach (const QString &family, database.families(QFontDatabase::SimplifiedChinese))
    {
        qDebug()<<family;
    }

    return a.exec();
}

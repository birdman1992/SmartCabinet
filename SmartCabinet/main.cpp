#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QFont>
#include <QWSServer>
#include "inputcontex/myinputpanelcontext.h"
#include "beautifului.h"

void customMessageHandler(QtMsgType type, const char *msg)
{
        QString txt;
        switch (type) {
        //调试信息提示
        case QtDebugMsg:
                txt = QString("Debug: %1").arg(msg);
                break;

        //一般的warning提示
        case QtWarningMsg:
                txt = QString("Warning: %1").arg(msg);
        break;
        //严重错误提示
        case QtCriticalMsg:
                txt = QString("Critical: %1").arg(msg);
        break;
        //致命错误提示
        case QtFatalMsg:
                txt = QString("Fatal: %1").arg(msg);
                abort();
        }

        QFile outFile("/home/debuglog.txt");
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifndef SIMULATE_ON
    QWSServer::setCursorVisible(false);
#endif
    MyInputPanelContext* inputContext = new MyInputPanelContext;
    QFile outFile("/home/debuglog.txt");
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
    QFont font;
    font.setFamily(("msyh"));
    a.setFont(font);
    MainWidget w;
    w.show();

    return a.exec();
}

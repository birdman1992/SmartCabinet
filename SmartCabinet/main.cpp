#include "mainwidget.h"
#include <QApplication>
#include <QTextCodec>
#include <QFont>

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

//    beautiful::SetUTF8Code();//--显示中文 utf-8
//    beautiful::SetStyle("image/black");//黑色风格
//    qInstallMsgHandler(customMessageHandler);
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

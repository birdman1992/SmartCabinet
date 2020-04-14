#include "temptopdf.h"
#include <QDebug>
#include <QPoint>
#include <QTextDocument>
#include <QDir>
#include <QFile>

TempToPdf::TempToPdf(QObject *parent) : QThread(parent)
{
    tManager = TempManager::manager();
    appPath = qApp->applicationDirPath();
    logPath = appPath +PATH_TEMP_LOG;
    outputPath = appPath + PATH_TEMP_PDF;
    downLoadPath = QString();
    weekList = QString::fromUtf8("星期一 星期二 星期三 星期四 星期五 星期六 星期日").split(' ', QString::SkipEmptyParts);
    stateList = QString::fromUtf8("正常 超温 低温").split(' ', QString::SkipEmptyParts);
    if(!QDir(outputPath).exists())
    {
        QDir dir;
        dir.mkdir(outputPath);
    }
}

void TempToPdf::creatTempPdf(QStringList devList, QDate beginDate, QDate endDate)
{
    taskNum = devList.count() * (beginDate.daysTo(endDate)+1);
    taskProgress = 0;
    int i = 0;
    emptyTask = true;
    recordName = QString("%1_%2.tar.bz2").arg(beginDate.toString("yyyyMMdd")).arg(endDate.toString("yyyyMMdd"));
    foreach(QString dev, devList)
    {
        qDebug()<<dev<<devList<<taskProgress;
        taskProgress = i * beginDate.daysTo(endDate);
        if(creatTempPdf(dev, beginDate, endDate))
            emptyTask = false;
        i++;
    }
    emit updateProgress(100);

//    taskProgress = 100;
//    emit taskFinish();
    pdfCreateFinish();
}

QByteArray TempToPdf::getDownloadPath()
{
    qDebug()<<downLoadPath;
    if(downLoadPath.isEmpty())
    {
        return QByteArray("{\
                              \"code\": -1,\
                              \"msg\":\"任务数据为空\"\
                          }");
    }
    if(emptyTask)
    {
        return QByteArray("{\
                              \"code\": -1,\
                              \"msg\":\"任务数据为空\"\
                          }");
    }

    QString str = QString("{\
                  \"code\": 0,\
                  \"msg\":\"success\",\
               \"data\":\"%1\"\
              }").arg(downLoadPath);
    return str.toLocal8Bit();
}

void TempToPdf::run()
{

}

bool TempToPdf::creatTempPdf(QString dev, QDate beginDate, QDate endDate)
{
    QString logPath = appPath + PATH_TEMP_LOG + dev + "/";
    QString devName = tManager->getDevName(dev);
    if(devName.isEmpty())
        devName = dev;
    QDir logDir(logPath);
    logDir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    logDir.setSorting(QDir::Name);
    QStringList logs = logDir.entryList();
//    qDebug()<<"logs"<<logs;

    if(logs.isEmpty())
        return false;
    foreach(QString f, logs)//过滤startDate前的数据文件
    {
        if((f >= beginDate.toString("yyyyMMdd")) && (f<=endDate.toString("yyyyMMdd")))
            continue;
        logs.removeOne(f);
    }

    if(logs.isEmpty())
        return false;

    beginDate = QDate::fromString(logs.first(),"yyyyMMdd");
    endDate = QDate::fromString(logs.last(),"yyyyMMdd");
    //init printer
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(outputPath+QString("/%1_%2_%3.pdf").arg(devName).arg(beginDate.toString("yyyy-MM-dd")).arg(endDate.toString("yyyy-MM-dd")));
    qDebug()<<"creatTempPdf"<<printer.pageRect().height();
    QPainter painter;
    painter.begin(&printer);
    QPoint point(10, 10);

    painter.drawText(point, QString("设备名称:%1  设备ID:%2").arg(devName).arg(dev));
    point.setY(point.y()+20);

    painter.drawText(point, "日期\t\t\t温度(℃)\t湿度(%)\t状态");
    point.setY(point.y()+20);
    qDebug()<<"logs"<<logs;

    foreach(QString f, logs)
    {
        QFile fData(logPath+f);
        if(!fData.open(QFile::ReadOnly))
            continue;

        QTime lastDataTime = QTime();

        while(1)
        {
            QString fLine = QString(fData.readLine());
            if(fLine.size()<10)
                break;
            QStringList datas = fLine.split(' ', QString::SkipEmptyParts);
            QDateTime cDate = QDateTime::fromString(f+datas.at(0), "yyyyMMddhh:mm:ss");
            if(cDate.time()<lastDataTime)
                continue;
            lastDataTime = cDate.time();
            QString msg = QString("%1\t%2\t%3\t%4").arg(cDate.toString("yyyy-MM-dd %1 hh:mm:ss").arg(weekList.at(cDate.date().dayOfWeek()-1))).arg(datas.at(2)).arg(datas.at(3)).arg(stateList.at(datas.at(1).toInt()));
//            qDebug()<<msg;
            painter.drawText(point, msg);
            point.setY(point.y()+20);
            if(point.y()>(printer.pageRect().height()-20))
            {
                printer.newPage();
                point.setY(10);
            }
        }
        fData.close();
        taskProgress++;
//        qDebug()<<"taskprogress"<<taskProgress<<taskNum<<(taskProgress*100/taskNum-1);
        emit updateProgress(taskProgress*100/taskNum-1);
    }
    return true;
}

void TempToPdf::pdfCreateFinish()
{
    QString cmd = QString("rm /opt/web/webpages/*.tar.bz2");
    QByteArray qba = cmd.toLocal8Bit();
    system(qba.data());
    QString pdfPath = qApp->applicationDirPath()+PATH_TEMP_PDF;

    cmd = QString("tar -zcvf /opt/web/webpages/%1 %2*.pdf").arg(recordName).arg(pdfPath);
    qba = cmd.toLocal8Bit();
    qDebug()<<"[tarProcess]"<<cmd;
    system(qba.data());

    CabinetConfig* config = CabinetConfig::config();

    downLoadPath = QString("http://%1/%2").arg(config->getIp()).arg(recordName);
    qDebug()<<"[downLoadPath]"<<downLoadPath;


//    writeData("ExportDevRecord", str.toUtf8());
}

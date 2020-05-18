#include "tempmain.h"
#include "ui_tempmain.h"
#include <QSpacerItem>
#include <QHostAddress>
#include <QDebug>
#include <fcntl.h>
#include <unistd.h>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>

TempMain::TempMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TempMain)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    curUser = NULL;
    fWatchdog = -1;
    ui->set->hide();
    ui->history->hide();
    weekList = QString::fromUtf8("星期一 星期二 星期三 星期四 星期五 星期六 星期日").split(' ', QString::SkipEmptyParts);
    tempManager = TempManager::manager();
    tempManager->configInit();

    cabConfig = CabinetConfig::config();
    cabConfig->configInit();
    ui->cabId->setText(cabConfig->getCabinetId());
//    ui->cabName->ssetText(cabConfig->getDepartName());
//    qDebug()<<"getCabinetId"<<cabConfig->getCabinetId();
    httpServer = new HttpServer(this);
    connect(httpServer, SIGNAL(updateDevInfo(QString)), this, SLOT(updateDevInfo(QString)));

    server = new QTcpServer(this);
    if(!server->listen(QHostAddress::Any, 9999))
    {
        qDebug()<<"[server creat failed]";
    }
    else
    {
        qDebug()<<"[server creat success]";
    }
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));
//    for(int j=0; j<1;j++)
//    {
//        for(int i=0; i<5; i++)
//        {
//            TempCase* case1 = new TempCase(this);
//            ui->caseLayout->addWidget(case1, j,i,1,1);
//        }
//    }
//    QSpacerItem* item = new QSpacerItem(184,190);
//    ui->caseLayout->addItem(item, 1,0,1,1);
//    watchdogStart();
    ui->frame_case->hide();
    ui->showStack->setCurrentIndex(0);
//    QStringList devList = tempManager->getTempRecordList();
//    foreach(QString dev, devList)
//    {
//        tempManager->creatTempData(dev,365,300);
//    }
}

TempMain::~TempMain()
{
    ::close(fWatchdog);
    delete ui;
}

void TempMain::watchdogStart()
{
    fWatchdog = -1;
    fWatchdog = open("/dev/watchdog", O_WRONLY);
    if (fWatchdog == -1)
    {
        qDebug()<<"[watchdog] watchdog start failed.";
        return;
    }
    qDebug()<<"[watchdog]"<<"start";
}

//void TempMain::updateCabInfo(QString id, QString name)
//{
//    ui->cabId->setText(id);
//    ui->cabName->setText(name);
//}

void TempMain::updateLoginUser(UserInfo *user)
{
    if(user == NULL)
        return;

    if(curUser != NULL)
        delete curUser;

    curUser = new UserInfo(*user);
    ui->hello->setText(QString("您好！%1").arg(user->name));
    if(curUser->post == "管理员")
    {
        ui->set->show();
        ui->history->show();
    }
    else
    {
        ui->set->hide();
        ui->history->hide();
    }
//    ui->hello->show();
}

void TempMain::logOut()
{
    ui->hello->clear();
    ui->set->hide();
    ui->history->hide();
    ui->history->setChecked(false);
    if(curUser != NULL)
    {
        delete curUser;
        curUser = NULL;
    }
}

void TempMain::updateMsg(QString msg)
{
    ui->msg->setText(msg);
}

void TempMain::updateScanData(QString scanData)
{
    ui->scanData->setText(scanData);
    qDebug()<<"updateScanData"<<scanData;
}

void TempMain::updateTime()
{
    QString strDate = QDate::currentDate().toString("yyyy/MM/dd\n")+weekList.at(QDate::currentDate().dayOfWeek()-1);
    ui->date->setText(strDate);
    ui->time->setText(QTime::currentTime().toString("hh:mm:ss"));
    if((QTime::currentTime().second() %10) == 0)
    {
//
        watchdogTimeout();
        if(map_case.isEmpty())
            return;
        foreach(TempCase* tCase, map_case)
        {
            tCase->checkOverTime();
        }
    }
//    QString picName = QString("/home/pics/%1").arg(QTime::currentTime().toString("hhmmss.png"));
//    QPixmap pic = QPixmap::grabWindow(QApplication::desktop()->winId());
//    bool rst = pic.save(picName, "PNG");
    //    qDebug()<<"savePic"<<picName<<rst;
}

TempCase *TempMain::getDevCase(QString devId)
{
    return map_case.value(devId, NULL);
}

void TempMain::paintHistory()
{
//绘制温度曲线
    QPen drawPen;
    QColor lineColor;
    drawPen.setColor(QColor(0, 255, 0, 50));
    drawPen.setWidth(1);
//    QCPGraph* graph_over = ui->tab_w->addGraph();
//    graph_over->setLineStyle(QCPGraph::lsLine);
//    graph_over->setPen(drawPen);
//    graph_over->setBrush(QBrush(QColor(255, 0, 0, 50)));
//    QCPGraph* graph_max = ui->tab_w->addGraph();
//    graph_max->setLineStyle(QCPGraph::lsLine);
//    graph_max->setBrush(QBrush(QColor(0, 255, 0, 50)));
//    QCPGraph* graph_min = ui->tab_w->addGraph();
//    graph_min->setLineStyle(QCPGraph::lsLine);
//    graph_over->setChannelFillGraph(graph_max);
//    graph_max->setChannelFillGraph(graph_min);
//    graph_max->setPen(drawPen);
//    graph_min->setPen(drawPen);

    foreach(TempCase* tCase, map_case)
    {
        lineColor.setNamedColor(tCase->devColor());
        drawPen.setColor(lineColor);
        QCPGraph* graph_w = ui->tab_w->addGraph();
        graph_w->setSelectable(QCP::stNone);
        graph_w->setPen(drawPen);
        graph_w->setLineStyle(QCPGraph::lsLine);
        graph_w->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 1));

        QCPGraph* graph_s = ui->tab_s->addGraph();
        graph_s->setSelectable(QCP::stNone);
        graph_s->setPen(drawPen);
        graph_s->setLineStyle(QCPGraph::lsLine);
        graph_s->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 1));

        if(tCase->creatHistoryData(QDate::currentDate().addDays(-7), 1))
        {
            graph_w->addData(tCase->dataTime(), tCase->dataTemp());
            graph_s->addData(tCase->dataTime(), tCase->dataHum());
        }
    }

    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("hh:mm\nMM/dd");
    dateTimeTicker->setDateTimeSpec(Qt::LocalTime);
    dateTimeTicker->setTickCount(15);
    ui->tab_w->xAxis->setTicker(dateTimeTicker);

//    ui->tab_w->xAxis->lowerEnding()
    QSharedPointer<QCPAxisTickerText> tempTicker(new QCPAxisTickerText);
    tempTicker->setSubTickCount(9);
    tempTicker->addTick(-10,"-10℃");
    tempTicker->addTick(0,"0℃");
    tempTicker->addTick(1,"1℃");
    tempTicker->addTick(2,"2℃");
    tempTicker->addTick(3,"3℃");
    tempTicker->addTick(4,"4℃");
    tempTicker->addTick(5,"5℃");
    tempTicker->addTick(6,"6℃");
    tempTicker->addTick(7,"7℃");
    tempTicker->addTick(8,"8℃");
    tempTicker->addTick(9,"9℃");
    tempTicker->addTick(10,"10℃");
    tempTicker->addTick(11,"11℃");
    tempTicker->addTick(12,"12℃");
    tempTicker->addTick(13,"13℃");
    tempTicker->addTick(14,"14℃");
    tempTicker->addTick(15,"15℃");
    tempTicker->addTick(16,"16℃");
    tempTicker->addTick(17,"17℃");
    tempTicker->addTick(18,"18℃");
    tempTicker->addTick(19,"19℃");
    tempTicker->addTick(20,"20℃");
    ui->tab_w->yAxis->setTicker(tempTicker);

    ui->tab_w->rescaleAxes();
    paintDataRange(8, 2);

//    ui->tab_w->xAxis->scaleRange(1.0, ui->tab_w->xAxis->range().center());
    ui->tab_w->yAxis->scaleRange(3.0, ui->tab_w->yAxis->range().center());
//    ui->tab_w->yAxis->setRangeUpper(20);
    ui->tab_w->yAxis->setRangeLower(-10);
    ui->tab_w->replot();

    QSharedPointer<QCPAxisTicker> humTicker(new QCPAxisTicker);
    humTicker->setTickCount(11);
    ui->tab_s->yAxis->setTicker(humTicker);
    ui->tab_s->xAxis->setTicker(dateTimeTicker);
    ui->tab_s->rescaleAxes();
    ui->tab_s->yAxis->setRangeLower(0);
    ui->tab_s->yAxis->setRangeUpper(100);
    ui->tab_s->replot();
}

void TempMain::paintHistory(TempCase *pCase)
{
    if(pCase == NULL)
        return;
    if(pCase->devId() == NULL)
        return;

    ui->showStack->setCurrentIndex(1);

    QPen drawPen;
    QColor lineColor;
    drawPen.setWidth(1);

    lineColor.setNamedColor(pCase->devColor());
    drawPen.setColor(lineColor);

    QCPGraph* graph_w = ui->tab_w->addGraph();
    graph_w->setSelectable(QCP::stNone);
    graph_w->setPen(drawPen);
    graph_w->setLineStyle(QCPGraph::lsLine);
    graph_w->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 1));

    QCPGraph* graph_s = ui->tab_s->addGraph();
    graph_s->setSelectable(QCP::stNone);
    graph_s->setPen(drawPen);
    graph_s->setLineStyle(QCPGraph::lsLine);
    graph_s->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone, 1));

    if(pCase->creatHistoryData(QDate::currentDate().addDays(-7), 1))
    {
        graph_w->addData(pCase->dataTime(), pCase->dataTemp());
        graph_s->addData(pCase->dataTime(), pCase->dataHum());
    }

    ui->tab_w->rescaleAxes();
    ui->tab_s->rescaleAxes();
    paintYAxis();

    ui->tab_w->yAxis->scaleRange(3.0, ui->tab_w->yAxis->range().center());
    ui->tab_w->yAxis->setRangeLower(-10);
    ui->tab_w->yAxis2->setRangeUpper(ui->tab_w->yAxis->range().upper);
    ui->tab_w->yAxis2->setRangeLower(-10);

    paintDataRange(pCase->maxTemp(), pCase->minTemp());
    ui->tab_w->replot();
    ui->tab_s->replot();
}

void TempMain::paintDataRange(int max, int min)
{
    QPen drawPen;
    drawPen.setColor(QColor(0, 0, 0, 0));
    drawPen.setWidth(1);
    QCPGraph* graph_over = ui->tab_w->addGraph();//温度上限
    graph_over->setPen(drawPen);
    graph_over->setLineStyle(QCPGraph::lsLine);
    graph_over->setBrush(QBrush(QColor(255, 0, 0, 50)));
    QCPGraph* graph_max = ui->tab_w->addGraph();
    graph_max->setPen(drawPen);
    graph_max->setLineStyle(QCPGraph::lsLine);
    graph_max->setBrush(QBrush(QColor(0, 255, 0, 50)));
    QCPGraph* graph_min = ui->tab_w->addGraph();
    graph_min->setPen(drawPen);
    graph_min->setLineStyle(QCPGraph::lsLine);
    graph_min->setBrush(QBrush(QColor(0, 150, 150, 50)));
    QCPGraph* graph_lower = ui->tab_w->addGraph();
    graph_lower->setLineStyle(QCPGraph::lsLine);
    graph_lower->setPen(drawPen);

    graph_over->setChannelFillGraph(graph_max);
    graph_max->setChannelFillGraph(graph_min);
    graph_min->setChannelFillGraph(graph_lower);

    graph_over->addData(ui->tab_w->xAxis->range().lower,ui->tab_w->yAxis->range().upper);
    graph_over->addData(QDateTime::currentDateTime().toTime_t(),ui->tab_w->yAxis->range().upper);
    graph_max->addData(ui->tab_w->xAxis->range().lower,max);
    graph_max->addData(QDateTime::currentDateTime().toTime_t(),max);
    graph_min->addData(ui->tab_w->xAxis->range().lower,min);
    graph_min->addData(QDateTime::currentDateTime().toTime_t(),min);
    graph_lower->addData(ui->tab_w->xAxis->range().lower,-10);
    graph_lower->addData(QDateTime::currentDateTime().toTime_t(),-10);
}

void TempMain::paintYAxis()
{
    QSharedPointer<QCPAxisTickerDateTime> dateTimeTicker(new QCPAxisTickerDateTime);
    dateTimeTicker->setDateTimeFormat("hh:mm\nMM/dd");
    dateTimeTicker->setDateTimeSpec(Qt::LocalTime);
    dateTimeTicker->setTickCount(15);
    ui->tab_w->xAxis->setTicker(dateTimeTicker);

//    ui->tab_w->xAxis->lowerEnding()
    QSharedPointer<QCPAxisTickerText> tempTicker(new QCPAxisTickerText);
    tempTicker->setSubTickCount(9);
    tempTicker->addTick(-10,"-10℃");
    tempTicker->addTick(-1,"-1℃");
    tempTicker->addTick(-2,"-2℃");
    tempTicker->addTick(-3,"-3℃");
    tempTicker->addTick(-4,"-4℃");
    tempTicker->addTick(-5,"-5℃");
    tempTicker->addTick(-6,"-6℃");
    tempTicker->addTick(-7,"-7℃");
    tempTicker->addTick(-8,"-8℃");
    tempTicker->addTick(-9,"-9℃");
    tempTicker->addTick(0,"0℃");
    tempTicker->addTick(1,"1℃");
    tempTicker->addTick(2,"2℃");
    tempTicker->addTick(3,"3℃");
    tempTicker->addTick(4,"4℃");
    tempTicker->addTick(5,"5℃");
    tempTicker->addTick(6,"6℃");
    tempTicker->addTick(7,"7℃");
    tempTicker->addTick(8,"8℃");
    tempTicker->addTick(9,"9℃");
    tempTicker->addTick(10,"10℃");
    tempTicker->addTick(11,"11℃");
    tempTicker->addTick(12,"12℃");
    tempTicker->addTick(13,"13℃");
    tempTicker->addTick(14,"14℃");
    tempTicker->addTick(15,"15℃");
    tempTicker->addTick(16,"16℃");
    tempTicker->addTick(17,"17℃");
    tempTicker->addTick(18,"18℃");
    tempTicker->addTick(19,"19℃");
    tempTicker->addTick(20,"20℃");
    tempTicker->addTick(30,"30℃");
    tempTicker->addTick(40,"40℃");
    ui->tab_w->yAxis->setTicker(tempTicker);
    ui->tab_w->yAxis2->setVisible(true);
    ui->tab_w->yAxis2->setTicker(tempTicker);

//    ui->tab_w->xAxis->scaleRange(1.0, ui->tab_w->xAxis->range().center());
//    ui->tab_w->yAxis->scaleRange(3.0, ui->tab_w->yAxis->range().center());
//    ui->tab_w->yAxis->setRangeUpper(20);
//    ui->tab_w->yAxis->setRangeLower(-10);
//    ui->tab_w->yAxis2->setRangeUpper(20);
//    ui->tab_w->yAxis2->setRangeLower(-10);


    QSharedPointer<QCPAxisTicker> humTicker(new QCPAxisTicker);
    humTicker->setTickCount(11);
    ui->tab_s->yAxis->setTicker(humTicker);
    ui->tab_s->xAxis->setTicker(dateTimeTicker);
    ui->tab_s->yAxis->setRangeLower(0);
    ui->tab_s->yAxis->setRangeUpper(100);
//    ui->tab_s->replot();
}

void TempMain::releaseHistory()
{
    ui->tab_s->clearGraphs();
    ui->tab_w->clearGraphs();
    foreach(TempCase* tCase, map_case)
    {
        tCase->clearHistoryData();
    }
}

void TempMain::frameCaseShow(TempCase *tCase)
{
    if(tCase == NULL)
        return;
    if(curUser == NULL)
        return;

    cur_case = tCase;

    ui->frame_case->show();
    ui->case_id->setText(tCase->devId());
    QString strName = tCase->devName();
    ui->case_name->setVisible(!strName.isEmpty());
    ui->case_name->setText(strName);
    ui->max_temp->setText(QString("%1").arg(tCase->maxTemp()));
    ui->min_temp->setText(QString("%1").arg(tCase->minTemp()));
    ui->warn_temp->setText(QString("%1").arg(tCase->warningTemp()));
    qDebug()<<curUser->post;
    if(curUser->post == "管理员")
    {
        ui->save_params->show();
        ui->max_temp->setReadOnly(false);
        ui->min_temp->setReadOnly(false);
        ui->warn_temp->setReadOnly(false);
    }
    else
    {
        ui->save_params->hide();
        ui->max_temp->setReadOnly(true);
        ui->min_temp->setReadOnly(true);
        ui->warn_temp->setReadOnly(true);
    }
}

void TempMain::updateCaseLayout()
{
    if(!list_case.isEmpty())
    {
        foreach (TempCase* tCase, list_case)
        {
            ui->caseLayout->removeWidget(tCase);
        }
        list_case.clear();
    }

    foreach(TempCase* tCase, map_case)
    {
//        tCase = map_case.value(caseId);
        if(list_case.indexOf(tCase) == -1)
            list_case<<tCase;
    }

    int i=0;
    foreach (TempCase* tCase, list_case)
    {
        ui->caseLayout->addWidget(tCase, i/8, i%8, 1, 1);
        i++;
    }
}

//#define TEST
void TempMain::newConnection()
{
    qDebug()<<"newConnection";
    QTcpSocket* skt = server->nextPendingConnection();
    TempCase* nCase;
#ifndef TEST
    if(map_case.contains(skt->peerAddress().toString()))
    {
        nCase = map_case.value(skt->peerAddress().toString());
        nCase->setSocket(skt);
    }
    else
    {
        nCase = new TempCase();

        connect(nCase, SIGNAL(caseClicked(TempCase*)), this, SLOT(caseClicked(TempCase*)));
        connect(nCase, SIGNAL(caseIdUpdate(TempCase*)), this, SLOT(caseIdUpdate(TempCase*)));
        nCase->setSocket(skt);
        map_case.insert(skt->peerAddress().toString(), nCase);
        updateCaseLayout();
    }
#else
    {
        nCase = new TempCase();
//        map_case.insert(skt->peerAddress().toString(), nCase);

        nCase->setSocket(skt);
        ui->caseLayout->addWidget(nCase, list_case.count()/8, list_case.count()%8, 1, 1);
        list_case<<nCase;
    }
#endif
}

void TempMain::updateDevInfo(QString dev)
{
    qDebug()<<"updateDevInfo";
    TempCase* tCase = map_case.value(dev, NULL);
    if(tCase == NULL)
        return;

    QString devName = tempManager->getDevName(dev);
    qDebug()<<"devName"<<devName;
    tCase->setCaseName(devName);
}

//void TempMain::deviceIdUpdate(TempCase *nCase)
//{
//    QString devIp = tempManager->getDeviceIp(nCase->devId());
//    if(devIp.isEmpty())
//        return;

//}

void TempMain::on_set_clicked()
{
    emit reqSetting();
}

void TempMain::on_history_clicked(bool checked)
{
    if(checked)
    {
        ui->showStack->setCurrentIndex(1);
        paintHistory();
    }
}

void TempMain::on_history_toggled(bool checked)
{
    if(!checked)
    {
        ui->showStack->setCurrentIndex(0);
        releaseHistory();
    }
}

void TempMain::caseClicked(TempCase *c)
{
    if(c == NULL)
        return;

    if(curUser == NULL)
        return;

    frameCaseShow(c);
    paintHistory(c);
}

void TempMain::caseIdUpdate(TempCase *tCase)
{
    qDebug()<<"[caseIdUpdate]";
    if(map_case.contains(tCase->devId()))
    {
        TempCase* oCase = map_case.value(tCase->devId());
        if(tCase != oCase)
        {
            ui->caseLayout->removeWidget(oCase);
            map_case.remove(tCase->devId());
            map_case.remove(tCase->devIp());
            map_case.insert(tCase->devId(), tCase);
            updateCaseLayout();
            qDebug()<<"[delete device map]"<<oCase->devId()<<oCase->devIp();
            qDebug()<<"[new device map]"<<tCase->devId()<<tCase->devIp();
            oCase->deleteLater();
        }
    }
    else
    {
        map_case.insert(tCase->devId(), tCase);
        qDebug()<<"[new device map]"<<tCase->devId()<<tCase->devIp();
    }
}

int TempMain::watchdogTimeout()
{
    int ret = 0;
//    qDebug()<<"[watchdog]"<<"write";
    if (fWatchdog != -1)
    {
        ret = write(fWatchdog, "a", 1);
    }
    return ret;
}


void TempMain::on_showStack_currentChanged(int arg1)
{
    if(arg1 == 1)
    {
        ui->history->setChecked(true);
    }
    else if(arg1 == 0)
    {
        ui->frame_case->hide();
    }
}

void TempMain::on_save_params_clicked()
{
    cur_case->setTempParams(ui->max_temp->text().toInt(), ui->min_temp->text().toInt(), ui->warn_temp->text().toInt());
}

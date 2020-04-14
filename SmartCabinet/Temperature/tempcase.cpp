#include "tempcase.h"
#include "ui_tempcase.h"
#include <QHostAddress>

TempCase::TempCase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TempCase)
{
    ui->setupUi(this);
    devManager = TempManager::manager();
    recorder = NULL;
    caseId = QString();
    caseName = QString();
    overTime = QDateTime::currentDateTime();
    lastRecordTime = QTime::currentTime();
    dev = NULL;
    curState = 0;
    tMax = 8;
    tMin = 2;
    tWarning = 16;
    tReport = 10;
    recordCount = 0;
    initNameMap();
    QByteArray test = QByteArray::fromHex("7e00");
    qDebug()<<"parTemp"<<parTemp(test);
}

//TempCase::TempCase(QWidget *parent, float maxTemp, float minTemp):
//    QWidget(parent),
//    ui(new Ui::TempCase)
//{
//    ui->setupUi(this);
//    tMax = maxTemp;
//    tMin = minTemp;
//    tCur = 0;
//    setCaseState(2);
//    ui->NO->setText(QString("BH%1").arg(caseId));
//}

TempCase::~TempCase()
{
    if(recorder != NULL)
    {
        delete recorder;
        recorder = NULL;
    }
    delete ui;
}

void TempCase::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void TempCase::initNameMap()
{
    if(!nameMap.isEmpty())
        return;
    nameMap.insert("2CF", QString::fromUtf8("临检室1号"));
    nameMap.insert("26D", QString::fromUtf8("临检室2号"));
    nameMap.insert("0D5", QString::fromUtf8("微生物室1号"));
    nameMap.insert("E0B", QString::fromUtf8("微生物室2号"));
    nameMap.insert("B29", QString::fromUtf8("生化室"));
    nameMap.insert("D4F", QString::fromUtf8("艾滋病初筛"));
    nameMap.insert("F14", QString::fromUtf8("发光室1号"));
    nameMap.insert("DE5", QString::fromUtf8("发光室(海尔)2号"));
    nameMap.insert("C44", QString::fromUtf8("血库1号"));
    nameMap.insert("E75", QString::fromUtf8("血库(海尔)2号"));
    nameMap.insert("C64", QString::fromUtf8("SPD仓(大)1号"));
    nameMap.insert("6B7", QString::fromUtf8("SPD仓(小)2号"));
//    nameMap.insert("ACD", QString::fromUtf8("测试室(海尔)2号"));
}

QString TempCase::getCaseName(QString id)
{
    qDebug()<<"getCaseName"<<id;
//    if(!nameMap.contains(id.right(3)))
//    {
//        QString strName = devManager->searchNameForIp(devIp());
//        if(strName.isEmpty())
//        {
//            ui->NO->hide();
//            return id;
//        }
//        else
//        {
//            return strName;
//        }
//    }
    caseName = devManager->getDevName(id);
    if(caseName.isEmpty())
    {
        ui->NO->hide();
    }
//    devManager->setDevName(caseId, nameMap.value(id.right(3)));

    return caseName;
}

void TempCase::setCaseName(QString name)
{
    caseName = name;
    if(name.isEmpty())
    {
        ui->NO->hide();
        ui->NO->setText(name);
    }
    else
    {
        ui->NO->show();
        ui->NO->setText(name);
    }
}

void TempCase::updateOverTime()
{
    overTime = QDateTime::currentDateTime().addSecs(15);
}

void TempCase::mouseReleaseEvent(QMouseEvent *)
{
    emit caseClicked(this);
}

void TempCase::updateTemp(float temp)
{
    tCur = temp;
    if(tCur > tMax)
    {
        setCaseState(1);
    }
    else if(tCur < tMin)
    {
        setCaseState(1);
    }
    else
    {
        setCaseState(0);
    }
    ui->temp->setText(QString("%1℃").arg(tCur));
}

void TempCase::updateHum(float hum)
{
    hCur = hum;
    ui->hum->setText(QString("%1%").arg(hum));
}

float TempCase::getCurTemp()
{
    return tCur;
}

float TempCase::getCurHum()
{
    return hCur;
}

int TempCase::getCurState()
{
    return curState;
}

QString TempCase::getCurStateStr()
{
    if(curState>4)
        return "UNKWON";
    QStringList stateList;
    stateList<<"N"<<"U"<<"UNKWON"<<"UNKWON"<<"O";
    return stateList.at(curState);
}



void TempCase::setSocket(QTcpSocket *t)
{
    if(t == NULL)
        return;

    if(dev != NULL)
        dev->deleteLater();

    dev = t;
    connect(dev, SIGNAL(readyRead()), this, SLOT(recvDevData()));
    connect(dev, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(devStateChanged(QAbstractSocket::SocketState)));
    caseId = devManager->searchDeviceForIp(dev->peerAddress().toString());
    qDebug()<<"[dev ip]"<<caseId<<dev->peerAddress().toString();

    setCaseId(caseId);
    //    setDevParam();
}

void TempCase::setTempParams(int _max, int _min, int _warning)
{
    tMax = _max;
    tMin = _min;
    tWarning = _warning;
    devManager->setDevMaxTemp(caseId, tMax);
    devManager->setDevMinTemp(caseId, tMin);
    devManager->setDevWarningTemp(caseId, tWarning);
    setDevParam();
}

QString TempCase::devId()
{
    return caseId;
}

QString TempCase::devColor()
{
    return QString("#%1").arg(caseId.left(6));
}

QString TempCase::devIp()
{
    if(dev == NULL)
        return QString();

    return dev->peerAddress().toString();
}

QString TempCase::devName()
{
    return caseName;
}

int TempCase::maxTemp()
{
    return tMax;
}

int TempCase::minTemp()
{
    return tMin;
}

int TempCase::warningTemp()
{
    return tWarning;
}

bool TempCase::creatHistoryData(QDate startDate, int)
{
    if(recorder == NULL)
        return false;

    QString dirPath = recorder->recDir();
    QDir dir(dirPath);
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList files = dir.entryList();
    if(files.isEmpty())
        return false;
    foreach(QString f, files)//过滤一个月前的数据文件
    {
        if(f > startDate.toString("yyyyMMdd"))
            break;
        files.removeOne(f);
    }
    foreach(QString f, files)
    {
        QFile fData(dirPath+f);
        if(!fData.open(QFile::ReadOnly))
            continue;

        while(1)
        {
            QString fLine = QString(fData.readLine());
            if(fLine.size()<10)
                break;
            QStringList datas = fLine.split(' ', QString::SkipEmptyParts);
            QString sDate = f+datas.at(0);
            times.push_back(QDateTime::fromString(sDate, "yyyyMMddhh:mm:ss").toTime_t());
            temps.push_back(datas.at(2).toDouble());
            hums.push_back(datas.at(3).toDouble());
        }
        fData.close();
    }
    qDebug()<<"creatHistoryData:"<<times.count();
    return true;
}

void TempCase::clearHistoryData()
{
    temps.clear();
    temps.squeeze();
    hums.clear();
    hums.squeeze();
    times.clear();
    times.squeeze();
    qDebug()<<"clearHistoryData:"<<times.count();
}

QVector<double> TempCase::dataTime()
{
    return times;
}

QVector<double> TempCase::dataHum()
{
    return hums;
}

QVector<double> TempCase::dataTemp()
{
    return temps;
}

void TempCase::startSet()
{
    ui->caseStack->setCurrentIndex(1);
}

void TempCase::checkOverTime()
{
    if(QDateTime::currentDateTime() > overTime)
        setCaseState(4);
}

void TempCase::setCaseState(int state)
{
    if(state == curState)
        return;

    curState = state;

    switch(state)//超温|低温|离线|超湿|低湿
    {
        case 0://正常 N
        this->setStyleSheet("#TempCase{border-image: url(:/image/temp/icon_state_normal.png);}");break;
        case 1://超温 U
        this->setStyleSheet("#TempCase{border-image: url(:/image/temp/icon_state_warning.png);}");break;
        case 4://离线 O
        this->setStyleSheet("#TempCase{border-image: url(:/image/temp/icon_state_offline.png);}");break;
    }
}

void TempCase::setCaseId(QString id)
{
    if(!id.isEmpty())//此ip有对应的设备
    {
        ui->ID->setText(QString("ID:%1").arg(caseId));
        ui->NO->setText(getCaseName(QString("%1").arg(caseId)));

        if(recorder != NULL)
            delete recorder;

        recorder = new TempRecorder(id);
        tMax = devManager->getDevMaxTemp(caseId);
        tMin = devManager->getDevMinTemp(caseId);
        tReport = devManager->getDevReportTime(caseId);
        tWarning = devManager->getDevWarningTemp(caseId);
        ui->w_max->setText(QString("%1").arg(tMax));
        ui->w_min->setText(QString("%1").arg(tMin));
        ui->w_time->setText(QString("%1").arg(tReport));
        QTimer::singleShot(2000, this, SLOT(setDevParam()));
        emit caseIdUpdate(this);
//        setDevParam();
    }
}

unsigned char TempCase::int2Byte(int val)
{
    bool lessZ = val<0;
    char ret = qAbs(val);
    if(ret > 30)
        ret = 30;

    if(lessZ)
        ret |= 0x1<<7;
    return ret;
}

QByteArray TempCase::getParamsBytes()
{
    QByteArray qba = QByteArray::fromHex("fd0a03021000460005ff");

    qba[3] = int2Byte(tMin);
    qba[4] = int2Byte(tWarning);
    return qba;
}

void TempCase::setDevParam()
{
    QByteArray qba = getParamsBytes();
    qDebug()<<"[setDevParam]"<<qba.toHex();
    dev->write(qba);
}

float TempCase::parTemp(QByteArray qba)
{
//    qDebug()<<"parTemp"<<qba.toHex();
    if(qba[1]>(char)9)
        qba[1] = 0;

    float ret = (qba[0] & 0x7f)+0.1*qba[1];
    if(qba[0] & 0x80)
        ret = -ret;
    return ret;
}

float TempCase::parHum(QByteArray qba)
{
//    qDebug()<<"parHum"<<qba.toHex();
    if(qba[1]>(char)9)
        qba[1] = 0;

    float ret = (qba[0] & 0x7f)+0.1*qba[1];
    if(qba[0] & 0x80)
        ret = -ret;
    return ret;
}

void TempCase::recvDevData()
{
    QByteArray qba = dev->readAll();
    qDebug()<<"[temp data]"<<QString("[%1]").arg(caseId)<<qba.toHex();
    if(qba.size() != (int)qba.at(1))
        return;
    if(((unsigned char)qba.at(0) != 0xfe) || ((unsigned char)qba.at(qba.size()-1) != 0xff))
        return;

    unsigned char tType = qba[2];
    updateOverTime();

    switch(tType)
    {
    case 0x01:
        caseId = QString(qba.mid(4,4).toHex()).toUpper();
        devManager->addTempDevice(dev->peerAddress().toString(), this);
        qDebug()<<"[dev ip]"<<caseId<<dev->peerAddress();
        dev->write(QByteArray::fromHex("fd050101ff"));
        setCaseId(caseId);
        break;

    case 0x02://  |FE|08|02|温|度|湿|度|FF|
        float temp = parTemp(qba.mid(3,2));
        float hum = parHum(qba.mid(5,2));
        updateTemp(temp);
        updateHum(hum);

        if(lastRecordTime.addSecs(300) <= QTime::currentTime() || (QTime::currentTime() < lastRecordTime))
        {
            lastRecordTime = QTime::currentTime();
            recordCount = 0;
            if(recorder != NULL)
                recorder->recordTemp(temp, hum, curState);
        }
//        dev->write(QByteArray::fromHex("fd050201ff"));break;
    }
}

void TempCase::devStateChanged(QAbstractSocket::SocketState state)
{
    qDebug()<<"[devStateChanged]"<<caseId<<state;
}

void TempCase::on_save_clicked(bool checked)
{
    Q_UNUSED(checked);
}

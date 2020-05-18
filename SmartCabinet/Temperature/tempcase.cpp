#include "tempcase.h"
#include "ui_tempcase.h"
#include <QHostAddress>
#include <QTextCodec>

TempCase::TempCase(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TempCase)
{
    ui->setupUi(this);
    ui->ID->hide();
    devManager = TempManager::manager();
    recorder = NULL;
    overTime = QDateTime::currentDateTime();
    lastRecordTime = QTime::currentTime();
    dev = NULL;
    curState = dev_normal;
    tMax = 8;
    tMin = 2;
    tWarning = 16;
    tReport = 10;
    recordCount = 0;
    initNameMap();
//    QByteArray test = QByteArray::fromHex("74302e7478743d22313233b2dcd1f4f6ce22ffffff");
//    pacToName(test);
//    qDebug()<<"parTemp"<<parTemp(test);
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

//协议返回: 控制字 错误码
void TempCase::pacBack(int8_t ctrlWd, int8_t errWd)
{
    QByteArray retPac = QByteArray::fromHex("FD0000FF");
    retPac[1] = ctrlWd;
    retPac[2] = errWd;
    qDebug()<<"[pacBack]"<<retPac.toHex();
    dev->write(retPac);
}

void TempCase::parseLogin(QByteArray qba)
{
    DevType dType = (DevType)qba.at(2);
    setTempDevType(dType);
    QByteArray pacName = qba.mid(3, qba.size() - 1);
    QString name = pacToName(pacName);
    setCaseName(name);
    setCaseId(name.toLocal8Bit().toHex());
    devManager->addTempDevice(dev->peerAddress().toString(), this);
    qDebug()<<"[dev ip]"<<caseName()<<dev->peerAddress();
    pacBack(qba.at(1), 0);
//    emit caseIdUpdate(this);
}

//float temp = parTemp(qba.mid(3,2));
//float hum = parHum(qba.mid(5,2));
//updateTemp(temp);
//updateHum(hum);

//if(lastRecordTime.addSecs(300) <= QTime::currentTime() || (QTime::currentTime() < lastRecordTime))
//{
//    lastRecordTime = QTime::currentTime();
//    recordCount = 0;
//    if(recorder != NULL)
//        recorder->recordTemp(temp, hum, curState);
//}
//        dev->write(QByteArray::fromHex("fd050201ff"));break;
void TempCase::parseReport(QByteArray qba)
{
//    DevType dType = (DevType)qba.at(2);
//    quint8 dataGroupCount = qba.at(3);//数据组数
    QVector<float>temp(4, 0);
    QVector<float>hum(4, 0);
    for(int i=0; i<4 ;i++)
    {
        temp[i] = parTemp(qba.mid(4+i*2, 2));
        hum[i] = parHum(qba.mid(6+2*i, 2));
    }
    updateTemp(temp);
    updateHum(hum);
    pacBack(qba.at(1), 0);
}

//t0.txt=”荣茂信息”0xFF 0xFF 0xFF
QString TempCase::pacToName(QByteArray qba)
{
    QByteArray nameBytes = qba.mid(8, qba.size() - 13);//荣茂信息
    QTextCodec* gbk = QTextCodec::codecForName("GB2312");
    QString name = gbk->toUnicode(nameBytes);
    qDebug()<<name;
    return name;
}

QByteArray TempCase::nameToPac(QString name)
{

}

void TempCase::setCaseName(QString name)
{
    m_caseName = name;
    if(m_caseName.isEmpty())
    {
        ui->NO->hide();
        ui->NO->setText(m_caseName);
    }
    else
    {
        ui->NO->show();
        ui->NO->setText(m_caseName);
    }
}

void TempCase::updateOverTime()
{
    overTime = QDateTime::currentDateTime().addSecs(15);
}

void TempCase::mouseReleaseEvent(QMouseEvent *)
{
    emit caseClicked(this);
//    if(curState == dev_temp_over)
    {
        alarmPause();
    }
}

void TempCase::updateTemp(QVector<float> temp)
{
    tCur = temp[0];
    if(tCur > tMax)
    {
        setCaseState(dev_temp_over);
    }
    else if(tCur < tMin)
    {
        setCaseState(dev_temp_under);
    }
    else
    {
        setCaseState(dev_normal);
    }
    ui->temp->setText(QString("%1℃").arg(tCur));
    update();
//    qDebug()<<tCur;
}

void TempCase::updateHum(QVector<float> hum)
{
    hCur = hum[0];
    ui->hum->setText(QString("%1%").arg(hCur));
//    qDebug()<<hCur;
}

float TempCase::getCurTemp()
{
    return tCur;
}

float TempCase::getCurHum()
{
    return hCur;
}

TempCase::DevState TempCase::getCurState()
{
    return curState;
}

//QString TempCase::getCurStateStr()
//{
//    if(curState>4)
//        return "UNKWON";
//    QStringList stateList;
//    stateList<<"N"<<"U"<<"UNKWON"<<"UNKWON"<<"O";
//    return stateList.at(curState);
//}



void TempCase::setSocket(QTcpSocket *t)
{
    if(t == NULL)
        return;

    if(dev != NULL)
        dev->deleteLater();

    dev = t;
    connect(dev, SIGNAL(readyRead()), this, SLOT(recvDevData()));
    connect(dev, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(devStateChanged(QAbstractSocket::SocketState)));
    QString name = devManager->searchDeviceForIp(dev->peerAddress().toString());
    setCaseName(name);
    qDebug()<<"[dev ip]"<<m_caseName<<dev->peerAddress().toString();
    setCaseId(m_caseName.toLocal8Bit().toHex());
}

void TempCase::setTempParams(int _max, int _min, int _warningm, int _report, bool soundOff)
{
    tMax = _max;
    tMin = _min;
    tWarning = _warningm;
    tReport = _report;
    setSoundOff(soundOff);
    devManager->setDevMaxTemp(m_caseId, tMax);
    devManager->setDevMinTemp(m_caseId, tMin);
    devManager->setDevWarningTemp(m_caseId, tWarning);
    devManager->setDevReportTime(m_caseId, tReport);
    devManager->setDevSoundOff(m_caseId, m_SoundOff);
    setDevParam();
}

QString TempCase::devId()
{
    return m_caseName;
}

QString TempCase::devColor()
{
    return QString("#%1").arg(caseId().right(6));
}

QString TempCase::devIp()
{
    if(dev == NULL)
        return QString();

    return dev->peerAddress().toString();
}

QString TempCase::devName()
{
    return caseName();
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

int TempCase::reportTime()
{
    return tReport;
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
    qDebug()<<"checkOverTime"<<QDateTime::currentDateTime()<<overTime;
    if(QDateTime::currentDateTime() > overTime)
        setCaseState(dev_offline);
}

void TempCase::setCaseState(DevState state)
{
    if(state == curState)
        return;

    curState = state;

    switch(state)//超温|低温|离线|超湿|低湿
    {
    case dev_normal://正常 N
        this->setStyleSheet("#TempCase{border-image: url(:/image/temp/icon_state_normal.png);}");break;
    case dev_temp_over://超温 U
        this->setStyleSheet("#TempCase{border-image: url(:/image/temp/icon_state_warning.png);}");break;
    case dev_offline://离线 O
        this->setStyleSheet("#TempCase{border-image: url(:/image/temp/icon_state_offline.png);}");break;
    default:
        break;
    }
}

void TempCase::setCaseId(QString id)
{
    if(!id.isEmpty())//此ip有对应的设备
    {
//        ui->ID->setText(QString("ID:%1").arg(caseId));
        ui->NO->setText(caseName());

        if(recorder != NULL)
            delete recorder;

        recorder = new TempRecorder(id);
        tMax = devManager->getDevMaxTemp(m_caseId);
        tMin = devManager->getDevMinTemp(m_caseId);
        tReport = devManager->getDevReportTime(m_caseId);
        tWarning = devManager->getDevWarningTemp(m_caseId);
        setSoundOff(devManager->getDevSoundOff(m_caseId));
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
    QByteArray qba = QByteArray::fromHex("fd0302000000460005ff");

    qba[2] = tempDevType();
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

void TempCase::alarmPause()
{
    QByteArray qba = QByteArray::fromHex("FD06010000000FFF");
    qDebug()<<"[alarmPause]"<<qba.toHex();
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
    qDebug()<<"[temp data]"<<QString("[%1]").arg(caseName())<<qba.toHex()<<dev->peerAddress().toString();
//    if(qba.size() != (int)qba.at(1))
//        return;
    if(((unsigned char)qba.at(0) != 0xfe) || ((unsigned char)qba.at(qba.size()-1) != 0xff))
        return;

//    return;
    unsigned char tType = qba[1];
    updateOverTime();

    switch(tType)
    {
    case 0x01:
        parseLogin(qba);
        break;

    case 0x02://  |FE|08|02|温|度|湿|度|FF|
        parseReport(qba);
        break;
    default:
        break;
    }
}

void TempCase::devStateChanged(QAbstractSocket::SocketState state)
{
    qDebug()<<"[devStateChanged]"<<caseName()<<state;
}

void TempCase::on_save_clicked(bool checked)
{
    Q_UNUSED(checked);
}

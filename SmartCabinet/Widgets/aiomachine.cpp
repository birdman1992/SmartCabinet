#include "aiomachine.h"
#include "ui_aiomachine.h"
#include <QDebug>

AIOMachine::AIOMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIOMachine)
{
    ui->setupUi(this);
    initNumLabel();
    sysLock();
    loginState = false;
    optUser = NULL;
    config = CabinetConfig::config();
    setAioInfo(config->getDepartName(), config->getCabinetId());
    if(config->getCabinetMode() == "aio")
    {
        sysTime = new QTimer(this);
        connect(sysTime, SIGNAL(timeout()), this, SLOT(updateTime()));
        sysTime->start(1000);
    }
}

AIOMachine::~AIOMachine()
{
    delete ui;
}

void AIOMachine::recvScanData(QByteArray)
{

}

void AIOMachine::recvUserCheckRst(UserInfo *user)
{
    optUser = user;
    loginState = true;
    if(config->getDepartName().isEmpty())
    {
        qDebug()<<user->departName;
        config->setDepartName(user->departName);
        setAioInfo(config->getDepartName(), config->getCabinetId());
    }

    ui->aio_hello->setText(QString("您好！%1").arg(user->name));
    sysUnlock();
}

void AIOMachine::recvUserInfo(QByteArray qba)
{
    if(loginState == true)
    {
        if(optUser->cardId == QString(qba))
            return;
    }
    ui->frame_quit->show();
    ui->aio_hello->show();
    ui->aio_hello->setText("正在识别");
    emit requireUserCheck(QString(qba));
    QTimer::singleShot(5000, this, SLOT(loginTimeout()));
}

void AIOMachine::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

bool AIOMachine::eventFilter(QObject *obj, QEvent *e)
{
    if(e->type() == QEvent::MouseButtonRelease)
    {
        cEvent eventNum = (cEvent)l_num_label.indexOf((QLabel*)obj);
        if(eventNum>=0)
        {
            qDebug()<<"[click event]"<<eventNum;
            emit click_event(eventNum);
        }
    }
    return QWidget::eventFilter(obj, e);
}

void AIOMachine::initNumLabel()
{
    if(!l_num_label.isEmpty())
    {
        qDeleteAll(l_num_label.begin(), l_num_label.end());
        l_num_label.clear();
    }

    l_num_label<<ui->num_expired;
    l_num_label<<ui->num_goods;
    l_num_label<<ui->num_today_in;
    l_num_label<<ui->num_today_out;
    l_num_label<<ui->num_warning_rep;
    l_num_label<<ui->lab_temp;
    l_num_label<<ui->lab_hum;

    ui->num_expired->installEventFilter(this);
    ui->num_goods->installEventFilter(this);
    ui->num_today_in->installEventFilter(this);
    ui->num_today_out->installEventFilter(this);
    ui->num_warning_rep->installEventFilter(this);
    ui->lab_temp->installEventFilter(this);
    ui->lab_hum->installEventFilter(this);
}

void AIOMachine::setAioInfo(QString departName, QString departId)
{
    ui->aio_info->setText(QString("%1 ID:%2").arg(departName).arg(departId));
}

void AIOMachine::sysLock()
{
    ui->frame_aio->hide();
    loginState = false;
    optUser = NULL;
//    ui->frame_quit->hide();
}

void AIOMachine::sysUnlock()
{
    ui->frame_aio->show();

    //    ui->frame_quit->show();
}

void AIOMachine::loginTimeout()
{
    if(loginState == false)
    {
        ui->aio_hello->clear();
    }
}

void AIOMachine::updateTime()
{
    ui->aio_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if((QDateTime::currentDateTime().time().hour() == 4) && (QTime::currentTime().minute() == 0))
    {
        qDebug("[update time]");
        emit reqCheckVersion(false);
    }
}

void AIOMachine::on_aio_quit_clicked()
{
    sysLock();
    ui->aio_hello->clear();
}

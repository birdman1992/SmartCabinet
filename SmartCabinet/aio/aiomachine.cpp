#include "aiomachine.h"
#include "ui_aiomachine.h"
#include <QDebug>
#define MAX_TABLE_ROW 15

AIOMachine::AIOMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AIOMachine)
{
    ui->setupUi(this);
    initNumLabel();
    initColMap();

    ui->page_overview->setWindowOpacity(1);
    ui->page_overview->setAttribute(Qt::WA_TranslucentBackground);
    ui->page_table->setWindowOpacity(1);
    ui->page_table->setAttribute(Qt::WA_TranslucentBackground);

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
    sysLock();
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

void AIOMachine::initColMap()
{
    if(!mapColName.isEmpty())
        mapColName.clear();

    mapColName.insert("物品编码", goodsId);
    mapColName.insert("物品名称", goodsName);
    mapColName.insert("包类型", packageType);
    mapColName.insert("生产商", proName);
    mapColName.insert("供应商", supplyName);
    mapColName.insert("规格", size);
    mapColName.insert("单位", unit);
    mapColName.insert("预警数量", threshold);
    mapColName.insert("最大数量", maxThreshold);
    mapColName.insert("包数", packageCount);
    mapColName.insert("耗材数量", goodsCount);
    mapColName.insert("效期天数", lifeDay);
    mapColName.insert("有效期至", lifeTime);
    mapColName.insert("生产日期", productTime);
    mapColName.insert("单价", price);
    mapColName.insert("总价", sumCount);
    mapColName.insert("入库数", aioInNum);
    mapColName.insert("出库数", aioOutNum);
    mapColName.insert("操作人", optName);
    mapColName.insert("操作时间", optTime);
    mapColName.insert("批次", batchNumber);
    mapColName.insert("条码", traceId);
    listColName = mapColName.keys();
}

void AIOMachine::setAioInfo(QString departName, QString departId)
{
    ui->aio_info->setText(QString("%1 ID:%2").arg(departName).arg(departId));
}

void AIOMachine::setNumLabel(AIOOverview *overview)
{
    ui->num_expired->setText(QString("%1\n近效期物品").arg(overview->lifeTimeGoodsCount));
    ui->num_goods->setText(QString("%1\n耗材品种（个）").arg(overview->chesetGoodsCount));
    ui->num_today_in->setText(QString("￥%1\n今日入库(元)").arg(overview->inSumCount));
    ui->num_today_out->setText(QString("￥%1\n今日出库(元)").arg(overview->outSumCount));
    ui->num_warning_rep->setText(QString("%1\n库存预警").arg(overview->warnGoodsCount));
//    ui->lab_temp->setText(QString("%1\n耗材品种（个）").arg(overview->chesetGoodsCount));
    //    ui->lab_hum->setText(QString("%1\n耗材品种（个）").arg(overview->chesetGoodsCount));
}

void AIOMachine::showTable(QString title, QStringList colNames, QList<GoodsInfo*>listInfo)
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->tab_title->setText(title);
    int rowCount = listInfo.count()>MAX_TABLE_ROW?MAX_TABLE_ROW:listInfo.count();
    int colCount = colNames.count();
    ui->info_table->setRowCount(rowCount);
    ui->info_table->setColumnCount(colCount);
    ui->info_table->setHorizontalHeaderLabels(colNames);
    int colIndex = 0;
    int rowIndex = 0;

    foreach (GoodsInfo* info, listInfo)
    {
        colIndex = 0;
        foreach (QString col, colNames)
        {
            ui->info_table->setItem(rowIndex, colIndex, new QTableWidgetItem(getGoodsInfoText(info, col)));
            colIndex++;
        }
        rowIndex++;
        if(rowIndex >= rowCount)
            break;
    }
}

void AIOMachine::showNumExpired(QList<GoodsInfo *> lInfo)
{
    QString title = "近效期物品";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(batchNumber);
    colNames<<listColName.at(size);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(productTime);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(lifeDay);
    colNames<<listColName.at(productTime);
    colNames<<listColName.at(lifeTime);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumGoods(QList<GoodsInfo *> lInfo)
{
    QString title = "库存物品";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(size);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(threshold);
    colNames<<listColName.at(maxThreshold);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumTodayIn(QList<GoodsInfo *> lInfo)
{
    QString title = "今日入库";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(size);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(aioInNum);
    colNames<<listColName.at(price);
    colNames<<listColName.at(sumCount);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumTodayOut(QList<GoodsInfo *> lInfo)
{
    QString title = "今日出库";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(size);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(batchNumber);
    colNames<<listColName.at(traceId);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(supplyName);
    colNames<<listColName.at(goodsCount);
    colNames<<listColName.at(price);
    colNames<<listColName.at(sumCount);
    colNames<<listColName.at(optName);
    colNames<<listColName.at(optTime);
    showTable(title, colNames, lInfo);
}

void AIOMachine::showNumWarningRep(QList<GoodsInfo *> lInfo)
{
    QString title = "库存预警";
    QStringList colNames;
    colNames<<listColName.at(goodsId);
    colNames<<listColName.at(goodsName);
    colNames<<listColName.at(size);
    colNames<<listColName.at(unit);
    colNames<<listColName.at(proName);
    colNames<<listColName.at(packageType);
    colNames<<listColName.at(packageCount);
    colNames<<listColName.at(goodsCount);
    colNames<<listColName.at(threshold);
    showTable(title, colNames, lInfo);
}

QString AIOMachine::getGoodsInfoText(GoodsInfo *info, QString key)
{
    colMark mark = mapColName.value(key, unknow);
    switch(mark)
    {
    case goodsId:return info->id;//物品编码
    case goodsName:return info->name;//物品名称
    case packageType:return QString("%1").arg(info->goodsType);//包类型
    case proName:return info->proName;//生产商
    case supplyName:return info->supName;//供应商
    case size:return info->size;//规格
    case unit:return info->unit;//单位
    case threshold:return QString("%1").arg(info->threshold);//预警数量
    case maxThreshold:return QString("%1").arg(info->maxThreshold);//最大数量
    default:
        break;
    }
    return QString();
}

QList<GoodsInfo *> AIOMachine::listPage(unsigned int pageNum)
{
    int firstIndex = pageNum * MAX_TABLE_ROW;
    
}

void AIOMachine::sysLock()
{
    ui->frame_aio->hide();
    ui->aio_hello->clear();
    loginState = false;
    optUser = NULL;
    emit reqUpdateOverview();
    //    ui->frame_quit->hide();
}

void AIOMachine::recvAioOverview(QString msg, AIOOverview *overview)
{
    if(overview == NULL)
    {
        qDebug()<<"[AioOverview]"<<msg;
        return;
    }
    setNumLabel(overview);
    delete overview;
}

void AIOMachine::recvAioData(QString msg, AIOMachine::cEvent e, QList<GoodsInfo *> lInfo)
{
    if(lInfo.isEmpty())//接口调用失败
    {
        qDebug()<<"[AIOMachine]recvAioData failed:"<<msg;
        return;
    }
    switch(e)
    {
    case AIOMachine::click_lab_hum:break;
    case AIOMachine::click_num_expired:break;
    case AIOMachine::click_num_goods :break;
    case AIOMachine::click_num_today_in :break;
    case AIOMachine::click_num_today_out :break;
    case AIOMachine::click_num_warning_rep :break;
    case AIOMachine::click_lab_temp :break;
    default:
        break;
    }
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

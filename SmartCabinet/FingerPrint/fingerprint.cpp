#include "fingerprint.h"
#include "ui_fingerprint.h"
#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QImage>

FingerPrint::FingerPrint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FingerPrint)
{
    ui->setupUi(this);
    cmdCache.clear();
    curUserCard.clear();
    curUserName.clear();
    waitSelCurModule = false;
    curUserCard = QString();
    curState = STATE_CHECK;
    curModule = 0;
    ledState = 3;
    curDev = -1;
    lockState = 0;

    manager_user = UserPrintManager::manager();
    ctrlSeq = manager_user->getSeqConfig();
    ctrlIndex = manager_user->getIndexConfig();

    socketCan = new QSocketCan(this);
    socketCan->start();
    connect(socketCan, SIGNAL(moduleActive(int)), this, SLOT(moduleActived(int)));
    connect(socketCan, SIGNAL(canData(int,QByteArray)), this, SLOT(recvFingerData(int,QByteArray)));
    connect(socketCan, SIGNAL(doorState(int, bool)), this, SLOT(doorStateChanged(int, bool)));
    connect(socketCan, SIGNAL(canDevOK()), this, SLOT(canDevScan()));


//    QTimer::singleShot(2000, this, SLOT(on_test_clicked()));

    btnState.addButton(ui->led_state_1, 0);
    btnState.addButton(ui->led_state_2, 1);
    btnState.addButton(ui->led_state_3, 2);
    btnState.addButton(ui->led_state_4, 3);
    btnState.addButton(ui->led_state_5, 4);

    connect(&btnState, SIGNAL(buttonClicked(int)), this, SLOT(ledStateChanged(int)));
    initRetCodeList();//错误码表
}

FingerPrint::~FingerPrint()
{
    delete ui;
}

void FingerPrint::addUser()
{
    curState = STATE_REG;
    waitSelCurModule = true;
    curUserCard = QString();
    recvCurCardId(QByteArray("100000AA"));
}

void FingerPrint::recvFingerData(int canId, QByteArray data)
{
    ResponsePack* rPack = new ResponsePack(data);
    qDebug("[FingerPrint] canId:0x%x cmd:0x%x errCode:0x%x %s", canId, rPack->rcm, rPack->ret, retCodeList[rPack->ret].toUtf8().data());
    if(rPack->ret != 0)
    {
        qDebug("[FingerPrint] canId:0x%x cmd:0x%x errCode:0x%x %s", canId, rPack->rcm, rPack->ret, retCodeList[rPack->ret].toUtf8().data());

        if(rPack->rcm == 0x63)
        {
            cmdSetLed(canId, 1, MODEL_ERROR);
            cmdSetLed(canId, 2, MODEL_NORMAL);
        }

        delete rPack;
        return;
    }

    switch(rPack->rcm)
    {
    case 0x01://CMD_TEST_CONNECTION
        if(canDevList.indexOf(canId) == -1)
        {
            canDevList<<canId;
            cmdSetLed(canId, 2, MODEL_NORMAL);
        }
        break;

    case 0x09://CMD_GET_MODULE_SN
        if(rPack->prefix == 0x5aa5)
        {
            if(canDevList.indexOf(canId) == -1)
            {
                canDevList<<canId;
                cmdSetLed(canId, 2, MODEL_NORMAL);
                qDebug()<<"[can dev]:"<<canId<<rPack->data;
            }
        }
        break;

    case 0x20://CMD_GET_IMAGE
        cmdGenerate(canId);//生成指纹模板
        break;

    case 0x22://CMD_UP_IMAGE_CODE
        if(rPack->prefix == 0x55aa)
        {
            char* pos = rPack->data.data();
            img_w = *(quint16*)pos;
            pos += 2;
            img_h = *(quint16*)pos;
            img_data.clear();
            qDebug("get img:%dx%d", img_w, img_h);
        }
        else if(rPack->prefix == 0x5aa5)
        {
//            qDebug()<<rPack->data.toHex();
//            img_data.append(rPack->data.right(data.length()-2));
//            QImage img = QImage((uchar*)img_data.data(), img_w, img_h, QImage::Format_Indexed8);

//            ui->img->setPixmap(QPixmap::fromImage(img));
//            qDebug("img data:%d", img_data.length());
        }
        break;

    case 0x40://CMD_STORE_CHAR
        boardCastIn++;
        if(boardCastIn == boardCastOut)
        {
            showMsg("指纹注册完成");
            curState = STATE_CHECK;
        }
        cmdSetLed(canId, 2, MODEL_NORMAL);
        break;

    case 0x42://CMD_UP_CHAR
        if(rPack->prefix == 0x55aa)
        {
            char* pos = rPack->data.data();
            temp_size = *(quint16*)pos;
            temp_data.clear();
            qDebug("get template:%d", temp_size);
        }
        else if(rPack->prefix == 0x5aa5)
        {
//            qDebug()<<rPack->data.toHex();
            temp_data = QByteArray(rPack->data);
//            qDebug("template data:%d", temp_data.length());
//            qDebug()<<temp_data.toHex();
            curFingerId = manager_user->getNewUserID(curUserCard);
            manager_user->setUserInfo(curFingerId, curUserCard, curUserName, temp_data);
//            temp_data = dataCheckSum(temp_data);
            templateDistr(temp_data);
        }
        break;

    case 0x43://CMD_DOWN_CHAR
        if(rPack->prefix == 0x55aa)
        {
            cmdWriteTemplate(canId, temp_data, 0, true);
        }
        else if(rPack->prefix == 0x5aa5)
        {
            cmdStoreChar(canId, curFingerId);
        }
        break;

    case 0x45://CMD_GET_EMPTY_ID

        break;

    case 0x60://CMD_GENERATE
        if(curState == STATE_REG)
        {
            generateStep++;
            if(generateStep == 3)
            {
                cmdMerge(canId);
            }
            else
            {
                ui->reg_progress->setValue(generateStep);
                cmdSetLed(canId, 1, MODEL_WAIT);
            }
        }
        else if(curState == STATE_CHECK)
        {
            cmdSearch(canId);
        }
//        cmdReadImage(canId, 1);
        break;

    case 0x61://CMD_MERGE
        ui->reg_progress->setValue(generateStep);
        cmdReadTemplate(canId);
        break;

    case 0x63://CMD_SEARCH
        userCheckPass(canId, *((quint16*)(rPack->data.data())));
        break;

    default:
        break;
    }
//    qDebug()<<data.toHex();
    delete rPack;
    sendCmdCache();
}

void FingerPrint::recvCurCardId(QByteArray cardId)
{
    curUserCard = QString(cardId);
    ui->cardId->setText(curUserCard);
}

void FingerPrint::ledStateChanged(int state)
{
    ledState = state;
}

void FingerPrint::moduleActived(int id)
{
    emit requireOpenLock(0, id);
    return;
    cmdSetLed(id, 1, MODEL_ACTIVE);//指纹进入激活状态

    switch(curState)
    {
    case STATE_REG:
        if(waitSelCurModule)
        {
            curModule = id;
            generateStep = 0;
            waitSelCurModule = false;
            foreach (int _id, canDevList)
            {
                if(id != _id)
                    cmdSetLed(_id, 2, MODEL_NORMAL);
            }
        }
        if(id != curModule)//屏蔽其它指纹模块
            return;

        if(generateStep < 3)
            cmdGetImage(id);//采集指纹图像
        break;

    case STATE_CHECK:
        generateStep = 0;
        cmdGetImage(id);
        break;
    default:
        break;
    }

}

void FingerPrint::doorStateChanged(int id, bool isOpen)
{
    if(curState == STATE_CHECK)
        cmdSetLed(id, 2, MODEL_NORMAL);

    int lastState = lockState;

    if(isOpen)
    {
        lockState |= (1<<id);
    }
    else
    {
        lockState &= ~(1<<id);
    }
    if(lastState != lockState)
        emit doorState(lockState);
}

void FingerPrint::userCheckPass(int canId, int fingerId)
{
    QByteArray cardId = manager_user->getCardId(fingerId);

    if(!cardId .isEmpty())
    {
        cmdSetLed(canId, 1, MODEL_PASS);
        qDebug()<<"userCheckPass"<<canId<<fingerId<<cardId;
        emit requireOpenLock(ctrlSeq[canId], ctrlIndex[canId]);
        emit userCardActive(cardId);
    }
    else
    {
        cmdSetLed(canId, 1, MODEL_ERROR);
    }
}

void FingerPrint::cmdGetImage(int id)
{
    CmdPack* packGetImg = new CmdPack(0x20);
//    addCmdCache(id, new QByteArray(packGetImg->packData()));
    socketCan->sendData(id, packGetImg->packData());
    delete packGetImg;
}

void FingerPrint::cmdReadImage(int id, char imgType)
{
    QByteArray data;
    data.resize(1);
    data[0] = imgType;

    CmdPack cmd(0x22, data);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdGenerate(int id)
{
    QByteArray cmdData;
    cmdData.resize(2);
    char* pos = cmdData.data();
    *(quint16*)pos = generateStep;
    CmdPack cmd(0x60, cmdData);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdMerge(int id, int mergeNum, int bufferId)
{
    showMsg("正在合成指纹数据");
    QByteArray cmdData;
    cmdData.resize(3);
    char* pos = cmdData.data();
    *(quint16*)pos = bufferId;
    pos += 2;
    *pos = mergeNum;
    CmdPack cmd(0x61, cmdData);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdReadTemplate(int id, int bufferId)
{
    showMsg("正在保存指纹数据");
    QByteArray cmdData;
    cmdData.resize(2);
    *(quint16*)cmdData.data() = bufferId;
    CmdPack cmd(0x42, cmdData);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdWriteTemplate(int id, QByteArray _data, int bufferId, bool isDataPack)
{
    QByteArray cmdData;

    if(isDataPack)
    {
        cmdData.resize(2);
        *(quint16*)cmdData.data() = bufferId;
        cmdData.append(_data);
        CmdPack cmd(0x43, cmdData);
        socketCan->sendData(id, cmd.packData());
//        qDebug()<<cmdData.toHex();
    }
    else
    {
        Q_UNUSED(bufferId);
        cmdData.resize(2);
        *(quint16*)cmdData.data() = _data.size()+2;
        CmdPack cmd(0x43, cmdData);
        socketCan->sendData(id, cmd.packData());
    }
}

void FingerPrint::templateDistr(QByteArray _data)
{
    showMsg("正在分发指纹数据");
    boardCastIn = 0;
    boardCastOut = 0;
//    _data = dataCheckSum(_data);

    foreach (int canId, canDevList)
    {
        boardCastOut++;
        cmdWriteTemplate(canId, _data);
    }
}

void FingerPrint::cmdStoreChar(int id, int tempID, int bufferId)
{
    cmdDeleteChar(id, tempID, tempID);

    QByteArray cmdData;
    cmdData.resize(4);
    char* pos = cmdData.data();
    *(quint16*)pos = tempID;
    pos += 2;
    *(quint16*)pos = bufferId;
    CmdPack cmd(0x40, cmdData);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdDeleteChar(int id, int rangeMin, int rangeMax)
{
    QByteArray cmdData;
    cmdData.resize(4);
    char* pos = cmdData.data();
    *(quint16*)pos = rangeMin;
    pos += 2;
    *(quint16*)pos = rangeMax;
    CmdPack cmd(0x44, cmdData);
    socketCan->sendData(id, cmd.packData());
}

//指纹模块id，显示状态(0:灭 1:亮 2:呼吸 3:慢闪烁 4快闪烁)，
void FingerPrint::cmdSetLed(int id, char state, char led)
{
    QByteArray cmdData;
    cmdData.resize(2);
    cmdData[0] = state;
    cmdData[1] = led | 0x80;

    CmdPack* cmd = new CmdPack(0x24, cmdData);
    socketCan->sendData(id, cmd->packData());
    delete cmd;
}

//指定编号范围的1：N 识别
void FingerPrint::cmdSearch(int id, int bufferId, int rangeMin, int rangeMax)
{
    QByteArray cmdData;
    cmdData.resize(6);
    char* pos = cmdData.data();
    *(quint16*)pos = bufferId;
    pos += 2;
    *(quint16*)pos = rangeMin;
    pos += 2;
    *(quint16*)pos = rangeMax;

    CmdPack cmd(0x63, cmdData);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdScanDev(int id)
{
    CmdPack cmd(0x01);
//    qDebug()<<"scan"<<id;
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdGetSN(int id)
{
    CmdPack cmd(0x09);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::cmdGetTempId(int id)
{
    QByteArray cmdData;
    cmdData.resize(4);
    char* pos = cmdData.data();
    *(quint16*)pos = 0;//范围起始
    pos += 2;
    *(quint16*)pos = 2000;//范围结束

    CmdPack cmd(0x45, cmdData);
    socketCan->sendData(id, cmd.packData());
}

void FingerPrint::on_led_r_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x0, ledState, LED_R);
    else
        cmdSetLed(0x0, 0, LED_R);
}

void FingerPrint::on_led_g_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x0, ledState, LED_G);
    else
    {
        cmdSetLed(0x0, 0, LED_G);
    }
}

void FingerPrint::on_led_b_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x0, ledState, LED_B);
    else
        cmdSetLed(0x0, 0, LED_B);
}

void FingerPrint::on_led_off_clicked()
{
    cmdSetLed(0x0, 0, LED_R|LED_G|LED_B);
}

void FingerPrint::initRetCodeList()
{
    retCodeList.clear();
    retCodeList.insert(0x00, "指令处理成功");
    retCodeList.insert(0x01, "指令处理失败");
    retCodeList.insert(0x10, "与指定编号中Template 的1:1 比对失败");
    retCodeList.insert(0x11, "已进行1:N 比对，但相同Template 不存在");
    retCodeList.insert(0x12, "在指定编号中不存在已注册的Template");
    retCodeList.insert(0x13, "在指定编号中已存在Template");
    retCodeList.insert(0x14, "不存在已注册的Template");
    retCodeList.insert(0x15, "不存在可注册的Template ID");
    retCodeList.insert(0x16, "不存在已损坏的Template");
    retCodeList.insert(0x17, "指定的Template Data 无效");
    retCodeList.insert(0x18, "该指纹已注册");
    retCodeList.insert(0x19, "指纹图像质量不好");
    retCodeList.insert(0x1a, "Template 合成失败");
    retCodeList.insert(0x1b, "没有进行通讯密码确认");
    retCodeList.insert(0x1c, "外部Flash 烧写出错");
    retCodeList.insert(0x1d, "指定Template 编号无效");
    retCodeList.insert(0x22, "使用了不正确的参数");
    retCodeList.insert(0x25, "指纹合成个数无效");
    retCodeList.insert(0x23, "在TimeOut 时间内没有输入指纹");
    retCodeList.insert(0x26, "Buffer ID 值不正确");
    retCodeList.insert(0x28, "采集器上没有指纹输入");
    retCodeList.insert(0x41, "指令被取消");
}

void FingerPrint::showMsg(QString msg)
{
    ui->msg->setText(msg);
}

void FingerPrint::addCmdCache(int id, QByteArray *cmd)
{
    cmdCache.insert(id, cmd);
}

void FingerPrint::sendCmdCache()
{
    if(cmdCache.isEmpty())
        return;

    int id = cmdCache.keys().at(0);
    QByteArray* data = cmdCache.take(id);
    socketCan->sendData(id, *data);
    delete data;
}

QByteArray *FingerPrint::getCmdCache(int id)
{
    QByteArray* cmd = cmdCache.value(id, NULL);
    if(cmd != NULL)
    {
        cmdCache.remove(id, cmd);
    }
    return cmd;
}

QByteArray FingerPrint::dataCheckSum(QByteArray inData)
{
    QByteArray outData = QByteArray(inData);
    outData.resize(outData.size()+2);
    int cks = 0;//清零，开始计算校验值
    for(int i=0; i<outData.size(); i++)
    {
        cks = (cks + outData[i]) & 0xffff;
    }
    *(quint16*)(outData.data() + outData.size() - 2) = cks;
    return outData;
}


void FingerPrint::on_led_r_2_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x0, ledState, LED_G|LED_B);
    else
        cmdSetLed(0x0, 0, LED_G|LED_B);
}

void FingerPrint::on_led_g_2_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x0, ledState, LED_R|LED_B);
    else
        cmdSetLed(0x0, 0, LED_R|LED_B);
}

void FingerPrint::on_led_b_2_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x0, ledState, LED_G|LED_R);
    else
        cmdSetLed(0x0, 0, LED_G|LED_R);
}

//检查0x0到0xf的can设备
void FingerPrint::canDevScan()
{
    for(int i=0; i<16; i++)
    {
//        cmdScanDev(i);
        cmdGetSN(i);
//        cmdSetLed(i, 2, );
    }
    socketCan->lockClear();
}

void FingerPrint::on_reg_clicked()
{
//    curUserCard = "A8F92201";
    curUserName = "Admin";
    if(curUserCard.isEmpty() || curUserName.isEmpty())
    {
        qDebug()<<"[FingerPrint] reg:no user info";
        showMsg("卡号为空");
        return;
    }
    curState = STATE_REG;
    waitSelCurModule = true;
    foreach (int id, canDevList)//指纹模块全部进入等待状态
    {
        cmdSetLed(id, 1, MODEL_WAIT);
    }
}

void FingerPrint::on_spin_can_valueChanged(int arg1)
{
    if((curDev != -1) && (canDevList.indexOf(curDev) != -1))
        cmdSetLed(curDev, 2, MODEL_NORMAL);

    curDev = arg1;
    ui->spin_seq->setValue(ctrlSeq[curDev]);
    ui->spin_index->setValue(ctrlIndex[curDev]);

    if(canDevList.indexOf(curDev) != -1)
        cmdSetLed(curDev, 1, MODEL_CONFIG);
}

void FingerPrint::on_spin_seq_valueChanged(int arg1)
{
    qDebug("231231234");
    if(curDev == -1)
    {
        curDev = ui->spin_can->value();
        cmdSetLed(curDev, 1, MODEL_CONFIG);
    }

    ctrlSeq[curDev] = arg1;
}

void FingerPrint::on_spin_index_valueChanged(int arg1)
{
    if(curDev == -1)
    {
        curDev = ui->spin_can->value();
        cmdSetLed(curDev, 1, MODEL_CONFIG);
    }

    ctrlIndex[curDev] = arg1;
}

void FingerPrint::on_save_config_clicked()
{
    cmdSetLed(curDev, 2, MODEL_NORMAL);
    curDev = -1;
    manager_user->setCtrlConfig(ctrlSeq, ctrlIndex);
}

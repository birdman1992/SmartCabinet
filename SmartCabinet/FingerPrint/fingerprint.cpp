#include "fingerprint.h"
#include "ui_fingerprint.h"
#include <QDebug>
#include <QTimer>

FingerPrint::FingerPrint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FingerPrint)
{
    ui->setupUi(this);
    cmdCache.clear();
    waitSelCurModule = false;
    curUserCard = QString();
    curState = STATE_CHECK;
    curModule = 0;
    ledState = 3;
    socketCan = new QSocketCan(this);
    socketCan->start();
    connect(socketCan, SIGNAL(moduleActive(int)), this, SLOT(moduleActived(int)));
    connect(socketCan, SIGNAL(canData(int,QByteArray)), this, SLOT(recvFingerData(int,QByteArray)));
    connect(socketCan, SIGNAL(doorState(int, bool)), this, SIGNAL(doorState(int, bool)));
//    QTimer::singleShot(2000, this, SLOT(on_test_clicked()));

    btnState.addButton(ui->led_state_1, 0);
    btnState.addButton(ui->led_state_2, 1);
    btnState.addButton(ui->led_state_3, 2);
    btnState.addButton(ui->led_state_4, 3);
    btnState.addButton(ui->led_state_5, 4);

    connect(&btnState, SIGNAL(buttonClicked(int)), this, SLOT(ledStateChanged(int)));
    initRetCodeList();
    for(int i=0; i<16; i++)
        cmdSetLed(i, 2, LED_B);
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
//    qDebug()<<"[FingerPrint] canId:"<<canId<<"cmd:"<<rPack->rcm<<rPack->ret<<;
    switch(rPack->rcm)
    {
    case 0x20://CMD_GET_IMAGE
        if(rPack->ret != 0)
            return;

        break;
    case 0x45://CMD_GET_EMPTY_ID
        if(rPack->ret != 4)
            return;
        generateStep = 0;
        curFingerId = QString(rPack->data.toHex().toUpper());
        qDebug()<<"[new finger id]"<<curFingerId;
        cmdGetImage(curModule);
        break;

    case 0x60://CMD_GENERATE
        if(rPack->ret != 0)
            return;
        //
        break;
    default:
        break;
    }
//    qDebug()<<data.toHex();
}

void FingerPrint::recvCurCardId(QByteArray cardId)
{
    curUserCard = QString(cardId);
}

void FingerPrint::ledStateChanged(int state)
{
    ledState = state;
}

void FingerPrint::moduleActived(int id)
{
    if(waitSelCurModule)
    {
        curModule = id;
        generateStep = 0;
        waitSelCurModule = false;
        cmdGetTempId(curModule);
    }
    emit requireOpenLock(0, id);
    QByteArray* cmd = getCmdCache(id);
    if(cmd == NULL)
        return;

    socketCan->sendData(id, *cmd);
    delete cmd;
    cmd = NULL;
}

void FingerPrint::on_test_clicked()
{
    cmdGetImage(1);
}

void FingerPrint::on_test_upload_clicked()
{

}

void FingerPrint::on_test_download_clicked()
{

}

void FingerPrint::cmdGetImage(int id)
{
    CmdPack* packGetImg = new CmdPack(0x20);
//    addCmdCache(id, new QByteArray(packGetImg->packData()));
    socketCan->sendData(id, packGetImg->packData());
    delete packGetImg;
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
        cmdSetLed(0x1, ledState, LED_R);
    else
        cmdSetLed(0x1, 0, LED_R);
}

void FingerPrint::on_led_g_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x1, ledState, LED_G);
    else
    {
        cmdSetLed(0x1, 0, LED_G);
    }
}

void FingerPrint::on_led_b_clicked(bool checked)
{
    if(checked)
        cmdSetLed(0x1, ledState, LED_B);
    else
        cmdSetLed(0x1, 0, LED_B);
}

void FingerPrint::on_led_off_clicked()
{
    cmdSetLed(0x1, 0, LED_R|LED_G|LED_B);
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

void FingerPrint::addCmdCache(int id, QByteArray *cmd)
{
    cmdCache.insert(id, cmd);
    cmdSetLed(id, 3, MODEL_WAIT);
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

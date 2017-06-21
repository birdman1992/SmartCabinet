#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include "defines.h"

#define LatticeNum 7  //定义药柜格子数

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

//    init_xiangang();

    init_huangpo();

}

void MainWidget::init_xiangang()
{
     ctrlUi = new ControlDevice;//控制台，接受型号
    QSettings set("Option.ini",QSettings::IniFormat);
    if(set.contains("qb"))
    {
        init_xg_ui_set();
        //--读取配置信息
        readSettings();
    }
    else
    {
        config_ui_set();

        connect(win_cabinet_set,SIGNAL(winSwitch(int)),this,SLOT(win_swich_2(int)));
        connect(win_cabinet_set,SIGNAL(setCabinet(QByteArray)),this,SLOT(set_cabinet(QByteArray)));
    }
}

void MainWidget::config_ui_set()
{
    cabinetConf = new CabinetConfig();

    //待机界面
    win_standby = new StandbyWidget(this);

    //用户管理界面
    win_user_manage = new UserWidget(this);
    win_user_manage->installGlobalConfig(cabinetConf);

    connect(win_user_manage, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_user_manage, SLOT(recvUserInfo(QByteArray)));

    //智能柜组合设置界面
    win_cabinet_set = new CabinetSet(this);

    ui->stackedWidget->addWidget(win_standby);
    ui->stackedWidget->addWidget(win_user_manage);
    ui->stackedWidget->addWidget(win_cabinet_set);
qDebug("3.1");
    if(cabinetConf->isFirstUse())
        ui->stackedWidget->setCurrentIndex(INDEX_USER_MANAGE);
    else
        ui->stackedWidget->setCurrentIndex(INDEX_CAB_SET);
    qDebug()<<"[currentIndex]"<<ui->stackedWidget->currentIndex();
}

void MainWidget::set_cabinet(QByteArray qb)
{
    qb_cabinet_order = qb;
        qDebug()<<"qb:"<<qb_cabinet_order.length();
}

void MainWidget::win_swich_2(int)
{
    init_xg_ui_set();
    num = qb_cabinet_order.length();
    for(int i = 0;i < num;i++)
    {
        qb_num[i] = qb_cabinet_order.toHex().at(2*i+1) - '0';
        switch (qb_num[i]) {
        case 1:
            btn_one();
            break;
        case 2:
            btn_two();
            break;
        case 3:
            btn_three();
            break;
        case 4:
            btn_four();
            break;
        default:
            break;
        }
    }
}


/**************************
 * 函 数 名：menu_set_init
 * 函数功能：stackwidget中菜单的初始化
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::menu_set_init()
{
    menu_widget = new SetMenu;
    stack->addWidget(menu_widget);
}

/**************************
 * 函 数 名：btn_one
 * 函数功能：添加药柜接口
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::btn_one()
{
    cabinets[1].show();
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        med.name = " ";
        med.cab_num = 1;
        med.lat_num = i;
        med .exist = 0;
        med.num = 0;
        medinf[1].append(med);
    }
}
void MainWidget::btn_two()
{
        cabinets[2].show();
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        med.name = " ";
        med.cab_num = 2;
        med.lat_num = i;
        med .exist = 0;
        med.num = 0;
        medinf[2].append(med);
    }
}
void MainWidget::btn_three()
{
        cabinets[3].show();
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        med.name = " ";
        med.cab_num = 3;
        med.lat_num = i;
        med .exist = 0;
        med.num = 0;
        medinf[3].append(med);
    }
}
void MainWidget::btn_four()
{
        cabinets[4].show();
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        med.name = " ";
        med.cab_num = 4;
        med.lat_num = i;
        med .exist = 0;
        med.num = 0;
        medinf[4].append(med);
    }
}

/**************************
 * 函 数 名：readSettings
 * 函数功能：读配置文件，读取程序状态数据等，在程序开始时执行
 * 参   数：无
 * 返 回 值：无
 * ***************************/
int MainWidget::readSettings()//读取程序设置
{
    //--读取主配置文件
    QSettings setting("Option.ini",QSettings::IniFormat);
    /*读取num药柜数并重新建立药柜*/
    if(setting.contains("num"))//--如果存在就读取
    {
        qb_cabinet_order = setting.value("qb").toByteArray();
        num = qb_cabinet_order.length();
        readSettings_cabinet(0);
        if(num > 1)
        {
            create_cabinet();
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

void MainWidget::create_cabinet()
{
    num = qb_cabinet_order.length();
    for(int i = 0;i < num;i++)
    {
        qb_num[i] = qb_cabinet_order.toHex().at(2*i+1) - '0';
        switch (qb_num[i]) {            //qb_num数组存放的是药柜创建的顺序
        case 1:
            btn_one();                           //创建药柜
            readSettings_cabinet(1);   //读取该药柜的配置文件
            break;
        case 2:
            btn_two();
            readSettings_cabinet(2);
            break;
        case 3:
            btn_three();
            readSettings_cabinet(3);
            break;
        case 4:
            btn_four();
            readSettings_cabinet(4);
            break;
        default:
            break;
        }
    }
}

void MainWidget::readSettings_cabinet(int row)
{
    QSettings cabinetsettings("cabinet" + QString::number(row,10) + ".ini",QSettings::IniFormat);
    cabinetsettings.beginGroup("cabinet");
    cabinetsettings.endGroup();

    /*读取每个格子的信息放入qlist；cab_lattice_num为使用的格子数*/
    for(int i = 0;i < LatticeNum; i++)
    {
        MedInf med;//--创建临时药品信息类
        cabinetsettings.beginGroup("lattice" + QString::number(i,10));
        med.num = cabinetsettings.value(QString::number(i,10) + "num").toInt();
        med.exist = cabinetsettings.value(QString::number(i,10) + "exist").toInt();
        med.cab_num = cabinetsettings.value(QString::number(i,10) + "cab_num").toInt();
        med.lat_num = cabinetsettings.value(QString::number(i,10) + "lat_num").toInt();
        med.name = cabinetsettings.value(QString::number(i,10) + "bbb").toString();
        med.application = cabinetsettings.value(QString::number(i,10) + "ccc").toString();
        med.ShelfLife = cabinetsettings.value(QString::number(i,10) + "ddd").toString();
        med.ProductionDate = cabinetsettings.value(QString::number(i,10) + "eee").toString();
        med.Features = cabinetsettings.value(QString::number(i,10) + "fff").toString();
        medinf[row].removeAt(i);
        medinf[row].insert(i,med); //--每读取一个格子的药品信息放入qlist链表中
        cabinetsettings.endGroup();

        //--恢复所有格子
        if(med.exist == 1)
        {
            cabinets[med.cab_num].item_add(med.lat_num,0,med.name);
        }
    }
}

/**************************
 * 函 数 名：writeSettings
 * 函数功能：写配置文件，记录程序状态数据等，在程序结束时执行
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::writeSettings()//保存程序设置
{
    //--配置主文件，记录药柜个数、路径
    QSettings settings("Option.ini",QSettings::IniFormat);
    settings.setValue("num",num);//--个数
    settings.setValue("path",path_cabinet);//--路径
    settings.setValue("qb",qb_cabinet_order);
    writeSettings_cabinet(0);
    for(int i = 0;i < num;i++)
    {
        switch (qb_num[i]) {
        case 1:
            writeSettings_cabinet(1);
            break;
        case 2:
            writeSettings_cabinet(2);
            break;
        case 3:
            writeSettings_cabinet(3);
            break;
        case 4:
            writeSettings_cabinet(4);
            break;
        default:
            break;
        }
    }
}

void MainWidget::writeSettings_cabinet(int row)
{
    //--为每个药柜建立配置文件保存药柜信息：格子数-lattice_num；已用格子数-cab_lattice_num
    QSettings cabinetsettings("cabinet" + QString::number(row,10) + ".ini",QSettings::IniFormat);

    cabinetsettings.beginGroup("cabinet");
    cabinetsettings.endGroup();
    /*先检查药柜是否为空，在写入每个格子的信息*/
    for(int i = 0;i < LatticeNum; i++)
    {
        //--lattice代表格子名称 i表示第几个格子，为每个格子分组
        int seral_num = medinf[row].at(i).num;
        cabinetsettings.beginGroup("lattice" + QString::number(i,10));
        cabinetsettings.setValue(QString::number(i,10) + "num",seral_num);
        cabinetsettings.setValue(QString::number(i,10) + "exist",medinf[row].at(i).exist);
        cabinetsettings.setValue(QString::number(i,10) + "cab_num",medinf[row].at(i).cab_num);
        cabinetsettings.setValue(QString::number(i,10) + "lat_num",medinf[row].at(i).lat_num);
        cabinetsettings.setValue(QString::number(i,10) + "bbb",medinf[row].at(i).name);
        cabinetsettings.setValue(QString::number(i,10) + "ccc",medinf[row].at(i).application);
        cabinetsettings.setValue(QString::number(i,10) + "ddd",medinf[row].at(i).Features);
        cabinetsettings.setValue(QString::number(i,10) + "eee",medinf[row].at(i).ProductionDate);
        cabinetsettings.setValue(QString::number(i,10) + "fff",medinf[row].at(i).ShelfLife);
        cabinetsettings.endGroup();
    }
}

/**************************
 * 函 数 名：check_code
 * 函数功能：绑定扫描型号的草函数，查询扫描的药品是否在药柜
 * 参   数：QByteArray qby--信号传递过来的药品名称
 * 返 回 值：无
 * ***************************/
void MainWidget::check_code(QByteArray qby)
{
    QString str = qby;
    int row = 0;
    int low = 0;
    bool exist = false;
    for( int j = 0 ;j < num; j++)
    {
        for( int i = 0 ;i < LatticeNum; i++)
        {
            if(medinf[qb_num[j]].at(i).name == str)
            {
                exist = true;//找到物品，记录位子：row--药柜 low--格子
                row = i;
                low = j;
            }
        }
    }

    if(exist == true)//存在药品，做出处理
    {
        show_inf->check_exist(medinf[low].at(row));
        show_inf->show();//show information
        ui_inf_exist = false;
        menu_widget->count_close();
    }
    else//不存在，做出处理
    {
        if(ui_inf_exist == true)//判断界面是否存在
        {
            MedInf med;
            med.name = str;
            med.exist = 0;
            med.cab_num = cabinet_num;
            med.lat_num = lattice_num;
            med.num = 0;
            show_inf->check_exist(med);
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("请选择要存放的药柜.");
            msgBox.exec();
            list->setCurrentRow(0);
        }
    }
}

/**************************
 * 函 数 名：read_showinf
 * 函数功能：绑定扫描型号的草函数，查询扫描的药品是否在药柜
 * 参   数：QByteArray qby--信号传递过来的药品名称
 * 返 回 值：无
 * ***************************/
void MainWidget::read_showinf(MedInf med)
{
    med.exist = 1;
    medinf[med.cab_num].removeAt(med.lat_num);
    medinf[med.cab_num].insert(med.lat_num,med);
    cabinets[med.cab_num].item_add(med.lat_num,0,med.name);
    show_inf->btn_close();
    ui_inf_exist = false;
    QMessageBox msgBox;
    msgBox.setText("存取完毕，请关好柜门！");
    msgBox.exec();
}

void MainWidget::check_pri_use()
{
    //--读取主配置文件
    QSettings setting("user/User.ini",QSettings::IniFormat);
    /*读取num药柜数并重新建立药柜*/
    setting.beginGroup("primary user");
    if(!setting.contains("name"))//--如果存在就读取
    {
//        Pri_user->setWindowModality(Qt::ApplicationModal);
        Pri_user->show();
    }
    else
    {
        UserInf user;
        user.name = setting.value("name").toString();
        user.authority = setting.value("authority").toInt();
        USER.append(user);
    }
    setting.endGroup();
}

void MainWidget::New_Pri_User(UserInf user)
{
     QSettings setting("user/User.ini",QSettings::IniFormat);
     setting.beginGroup("primary user");
     setting.setValue("name",user.name);
     setting.setValue("authority",user.authority);
     setting.endGroup();
}
void MainWidget::scan_user(QByteArray qb)
{
    Pri_user->scan_user_inf(qb);
}

void MainWidget::init_xg_ui_set()
{
    num = 1;//--初始一个药柜
    lattice_num = 7;

    ui_inf_exist = false;
    ui->stackedWidget->close();
    stack = new QStackedWidget(this); //为这个主窗体创建一个堆栈窗体
    list = new QListWidget(this);   //创建一个列表框
    qhbox_main = new QHBoxLayout;
    //往这个列表框里面添加元素
    list->insertItem(0, tr("主界面"));
    list->insertItem(1, tr("菜单"));
    list->insertItem(2, tr("简介"));
    list->item(0)->setSizeHint(QSize(100,100));
    list->item(1)->setSizeHint(QSize(100,100));
    list->item(2)->setSizeHint(QSize(100,100));
    list->item(0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    list->item(1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    list->item(2)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    list->setMinimumWidth(150);
    list->setMinimumHeight(400);

    qvbox_zero_layout = new QVBoxLayout();
    qvbox_one_layout = new QVBoxLayout();
    qvbox_two_layout = new QVBoxLayout();
    qvbox_three_layout = new QVBoxLayout();
    qvbox_four_layout = new QVBoxLayout();

    qvbox_zero_layout->addWidget(&cabinets[0]);
    qvbox_one_layout->addWidget(&cabinets[1]);          //--重新加入药柜
    qvbox_two_layout->addWidget(&cabinets[2]);          //--重新加入药柜
    qvbox_three_layout->addWidget(&cabinets[3]);          //--重新加入药柜
    qvbox_four_layout->addWidget(&cabinets[4]);          //--重新加入药柜
    cabinets[1].hide();
    cabinets[2].hide();
    cabinets[3].hide();
    cabinets[4].hide();
    qhbox_main->addLayout(qvbox_three_layout);
    qhbox_main->addLayout(qvbox_one_layout);
    qhbox_main->addLayout(qvbox_zero_layout);
    qhbox_main->addLayout(qvbox_two_layout);
    qhbox_main->addLayout(qvbox_four_layout);
    qhbox_main->setStretchFactor(qvbox_three_layout,1);
    qhbox_main->setStretchFactor(qvbox_one_layout,1);
    qhbox_main->setStretchFactor(qvbox_zero_layout,1);
    qhbox_main->setStretchFactor(qvbox_two_layout,1);
    qhbox_main->setStretchFactor(qvbox_four_layout,1);

    cab_widget = new QWidget;
    cab_widget->setLayout(qhbox_main);
    stack->addWidget(cab_widget);
    menu_set_init();
    //--set 每个药柜格子数
    cabinets[0].Cabinet_lattice_num_set(LatticeNum);
    cabinets[1].Cabinet_lattice_num_set(LatticeNum);
    cabinets[2].Cabinet_lattice_num_set(LatticeNum);
    cabinets[3].Cabinet_lattice_num_set(LatticeNum);
    cabinets[4].Cabinet_lattice_num_set(LatticeNum);

    //--为主药柜建立列表
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        med.name = "无";
        med.cab_num = 0;
        med.lat_num = i;
        med .exist = 0;
        med.num = 0;
        medinf[0].append(med);
    }

    ui->caseLayout->addWidget(list);  //把list里面的内容加到窗体里面
    ui->caseLayout->addWidget(stack, 0, Qt::AlignHCenter);
    ui->caseLayout->setStretchFactor(list, 1);  //设定为可伸缩的控件，第一个参数是用于指定设置的控件，第二个大于0表示这个控件可伸缩
    ui->caseLayout->setStretchFactor(stack, 5);

    connect(list, SIGNAL(currentRowChanged(int)), stack, SLOT(setCurrentIndex(int)));

//    ctrlUi = new ControlDevice;//控制台，接受型号
    connect(ctrlUi,SIGNAL(codeScanData(QByteArray)),this,SLOT(check_code(QByteArray)));
    connect(ctrlUi,SIGNAL(cardReaderData(QByteArray)),this,SLOT(scan_user(QByteArray)));
    show_inf = new ShowInf;//显示性息窗口
    connect(show_inf,SIGNAL(cabinet_inf(MedInf)),this,SLOT(read_showinf(MedInf)));
    Pri_user = new PrimaryUser;
    connect(Pri_user,SIGNAL(new_pri_user(UserInf)),this,SLOT(New_Pri_User(UserInf)));
    connect(&cabinets[0],SIGNAL(lattice_inf(int)),this,SLOT(cabinet_cleck(int)));
    connect(&cabinets[1],SIGNAL(lattice_inf(int)),this,SLOT(cabinet_cleck_one(int)));
    connect(&cabinets[2],SIGNAL(lattice_inf(int)),this,SLOT(cabinet_cleck_two(int)));
    connect(&cabinets[3],SIGNAL(lattice_inf(int)),this,SLOT(cabinet_cleck_three(int)));
    connect(&cabinets[4],SIGNAL(lattice_inf(int)),this,SLOT(cabinet_cleck_four(int)));

    check_pri_use();
}

void MainWidget::cabinet_cleck(int num)
{
        show_inf->check_exist(medinf[0].at(num));
        show_inf->show();
        ui_inf_exist = true;
        lattice_num = num;
        cabinet_num = 0;
}
void MainWidget::cabinet_cleck_one(int num)
{
        show_inf->check_exist(medinf[1].at(num));
        show_inf->show();
        ui_inf_exist = true;
        lattice_num = num;
        cabinet_num = 1;
}
void MainWidget::cabinet_cleck_two(int num)
{
        show_inf->check_exist(medinf[2].at(num));
        show_inf->show();
        ui_inf_exist = true;
        lattice_num = num;
        cabinet_num = 2;
}
void MainWidget::cabinet_cleck_three(int num)
{
        show_inf->check_exist(medinf[3].at(num));
        show_inf->show();
        ui_inf_exist = true;
        lattice_num = num;
        cabinet_num = 3;
}
void MainWidget::cabinet_cleck_four(int num)
{
        show_inf->check_exist(medinf[4].at(num));
        show_inf->show();
        ui_inf_exist = true;
        lattice_num = num;
        cabinet_num = 4;
}

void MainWidget::init_huangpo()
{
    qDebug("init_huangpo<<server");
    //智能柜配置
    cabinetConf = new CabinetConfig();

    //http通信类
    cabServer = new CabinetServer(this);
    cabServer->installGlobalConfig(cabinetConf);

    //仿真控制台
    ctrlUi = new ControlDevice;

    //扫码输入面板
    win_coder_keyboard = new coderKeyboard();
    connect(win_coder_keyboard, SIGNAL(coderData(QByteArray)), ctrlUi, SIGNAL(codeScanData(QByteArray)));

    //服务界面
    win_cab_service = new CabinetService();
    connect(win_cab_service, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));

    //智能柜展示界面
    win_cabinet = new CabinetWidget(this);
    win_cabinet->installGlobalConfig(cabinetConf);
    connect(ctrlUi, SIGNAL(codeScanData(QByteArray)), win_cabinet, SLOT(recvScanData(QByteArray)));
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_cabinet, SLOT(recvUserInfo(QByteArray)));
    connect(win_cabinet, SIGNAL(requireOpenCase(int,int)), ctrlUi, SLOT(openLock(int,int)));
    connect(win_cabinet, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(win_cabinet, SIGNAL(requireUserCheck(QString)), cabServer, SLOT(userLogin(QString)));
    connect(win_cabinet, SIGNAL(requireGoodsListCheck(QString)), cabServer, SLOT(listCheck(QString)));
    connect(win_cabinet, SIGNAL(requireCaseBind(int,int,QString)), cabServer, SLOT(cabinetBind(int,int,QString)));
    connect(win_cabinet, SIGNAL(goodsAccess(CaseAddress,QString,int,bool)), cabServer, SLOT(goodsAccess(CaseAddress,QString,int,bool)));
    connect(cabServer, SIGNAL(loginRst(UserInfo)), win_cabinet, SLOT(recvUserCheckRst(UserInfo)));
    connect(cabServer, SIGNAL(listRst(GoodsList*)), win_cabinet, SLOT(recvListInfo(GoodsList*)));
    connect(cabServer, SIGNAL(bindRst(bool)), win_cabinet, SLOT(recvBindRst(bool)));
    connect(cabServer, SIGNAL(goodsNumChanged(QString,int)), win_cabinet, SLOT(recvGoodsNumInfo(QString,int)));

    //待机界面
    win_standby = new StandbyWidget(this);
    win_standby->installGlobalConfig(cabinetConf);
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_standby, SLOT(recvUserInfo(QByteArray)));
    connect(win_standby, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));

    //用户管理界面
    win_user_manage = new UserWidget(this);
    win_user_manage->installGlobalConfig(cabinetConf);
    connect(win_user_manage, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ctrlUi, SIGNAL(cardReaderData(QByteArray)), win_user_manage, SLOT(recvUserInfo(QByteArray)));

    //智能柜组合设置界面
    win_cabinet_set = new CabinetSet(this);
    win_cabinet_set->installGlobalConfig(cabinetConf);
    connect(win_cabinet_set, SIGNAL(winSwitch(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(win_cabinet_set, SIGNAL(cabinetCreated()), win_cabinet, SLOT(cabinetInit()));
//    connect(win_cabinet_set, SIGNAL(setCabinet(QByteArray)), cabinetConf, SLOT(creatCabinetConfig(QByteArray)));

    ui->stackedWidget->addWidget(win_standby);
    ui->stackedWidget->addWidget(win_user_manage);
    ui->stackedWidget->addWidget(win_cabinet_set);
    ui->stackedWidget->addWidget(win_cabinet);
    ui->stackedWidget->addWidget(win_cab_service);

    if(cabinetConf->isFirstUse())
    {
//        if(cabinetConf->list_user.count())
            ui->stackedWidget->setCurrentIndex(INDEX_CAB_SET);
//        else
//            ui->stackedWidget->setCurrentIndex(INDEX_USER_MANAGE);
    }
    else
    {
        ui->stackedWidget->setCurrentIndex(INDEX_CAB_SHOW);
        win_cabinet->panel_init(cabinetConf->list_cabinet);
    }
    qDebug()<<"[currentIndex]"<<ui->stackedWidget->currentIndex();
    qDebug()<<cabinetConf->list_cabinet.count();
}

MainWidget::~MainWidget()
{
    delete ui;
    //--写入配置信息
    writeSettings();
}

void MainWidget::paintEvent(QPaintEvent*)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

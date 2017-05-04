#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>

#define LatticeNum 7  //定义药柜格子数

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    num = 1;//--初始一个药柜
    lattice_num = 7;
    cab_lattice_num[0]  = 0; //--每个药柜格子使用数初始化
    cab_lattice_num[1]  = 0;
    cab_lattice_num[2]  = 0;
    cab_lattice_num[3]  = 0;
    cab_lattice_num[4]  = 0;

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

    //--添加药柜btn
    btn_cabinet_add_one = new QPushButton("add");
    btn_cabinet_add_one->setMinimumWidth(200);
    btn_cabinet_add_one->setMinimumHeight(400);
    btn_cabinet_add_two = new QPushButton("add");
    btn_cabinet_add_two->setMinimumWidth(200);
    btn_cabinet_add_two->setMinimumHeight(400);
    btn_cabinet_add_three = new QPushButton("add");
    btn_cabinet_add_three->setMinimumWidth(200);
    btn_cabinet_add_three->setMinimumHeight(400);
    btn_cabinet_add_four = new QPushButton("add");
    btn_cabinet_add_four->setMinimumWidth(200);
    btn_cabinet_add_four->setMinimumHeight(400);
    qvbox_zero_layout = new QVBoxLayout();
    qvbox_one_layout = new QVBoxLayout();
    qvbox_two_layout = new QVBoxLayout();
    qvbox_three_layout = new QVBoxLayout();
    qvbox_four_layout = new QVBoxLayout();

    qvbox_zero_layout->addWidget(&cabinets[0]);
    qvbox_one_layout->addWidget(btn_cabinet_add_one);
    qvbox_two_layout->addWidget(btn_cabinet_add_two);
    qvbox_three_layout->addWidget(btn_cabinet_add_three);
    qvbox_four_layout->addWidget(btn_cabinet_add_four);
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
        medinf[0].append(med);
    }
    //--读取配置信息
    readSettings();

    ui->caseLayout->addWidget(list);  //把list里面的内容加到窗体里面
    ui->caseLayout->addWidget(stack, 0, Qt::AlignHCenter);
    ui->caseLayout->setStretchFactor(list, 1);  //设定为可伸缩的控件，第一个参数是用于指定设置的控件，第二个大于0表示这个控件可伸缩
    ui->caseLayout->setStretchFactor(stack, 5);

    connect(list, SIGNAL(currentRowChanged(int)), stack, SLOT(setCurrentIndex(int)));
    connect(btn_cabinet_add_one,SIGNAL(clicked(bool)),this,SLOT(btn_one()));
    connect(btn_cabinet_add_two,SIGNAL(clicked(bool)),this,SLOT(btn_two()));
    connect(btn_cabinet_add_three,SIGNAL(clicked(bool)),this,SLOT(btn_three()));
    connect(btn_cabinet_add_four,SIGNAL(clicked(bool)),this,SLOT(btn_four()));

    ctrlUi = new ControlDevice;//控制台，接受型号
    connect(ctrlUi,SIGNAL(codeScanData(QByteArray)),this,SLOT(check_code(QByteArray)));
    connect(ctrlUi,SIGNAL(cardReaderData(QByteArray)),this,SLOT(scan_user(QByteArray)));
    show_inf = new ShowInf;//显示性息窗口
    connect(show_inf,SIGNAL(cabinet_inf(MedInf)),this,SLOT(read_showinf(MedInf)));
    Pri_user = new PrimaryUser;
    connect(Pri_user,SIGNAL(new_pri_user(UserInf)),this,SLOT(New_Pri_User(UserInf)));

     check_pri_use();
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
    qvbox_one_layout->removeWidget(btn_cabinet_add_one);//--移除btn控件
    btn_cabinet_add_one->deleteLater();
    qvbox_one_layout->addWidget(&cabinets[1]);          //--重新加入药柜
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        medinf[1].append(med);
    }
    num++;
}
void MainWidget::btn_two()
{
    qvbox_two_layout->removeWidget(btn_cabinet_add_two);
    btn_cabinet_add_two->deleteLater();
    qvbox_two_layout->addWidget(&cabinets[2]);
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        medinf[2].append(med);
    }
    num++;
}
void MainWidget::btn_three()
{
    qvbox_three_layout->removeWidget(btn_cabinet_add_three);
    btn_cabinet_add_three->deleteLater();
    qvbox_three_layout->addWidget(&cabinets[3]);
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        medinf[3].append(med);
    }
    num++;
}
void MainWidget::btn_four()
{
    qvbox_four_layout->removeWidget(btn_cabinet_add_four);
    btn_cabinet_add_four->deleteLater();
    qvbox_four_layout->addWidget(&cabinets[4]);
    for(int i = 0;i < LatticeNum;i++)
    {
        MedInf med;
        medinf[4].append(med);
    }
    num++;
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
        num = setting.value("num").toInt();
        qDebug()<<"num:"<<num;
        if(num > 1)
        {
            int j = num - 1;
            for(int i = 1;i <= j;i++)
            {
                num = i;
                //--重新恢复药柜
                switch (i) {
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

        /*读取每个药柜的信息 num为药柜数量*/
        for(int j = 0;j < num; j++)
        {
            QSettings cabinetsettings("cabinet" + QString::number(j,10) + ".ini",QSettings::IniFormat);
            cabinetsettings.beginGroup("cabinet");
            lattice_num = cabinetsettings.value("LatticeNum").toInt();
            cab_lattice_num[j] = cabinetsettings.value("UsedLatticeNum").toInt();
            cabinetsettings.endGroup();

            /*读取每个格子的信息放入qlist；cab_lattice_num为使用的格子数*/
            for(int i = 0;i < LatticeNum; i++)
            {
                MedInf med;//--创建临时药品信息类
                cabinetsettings.beginGroup("lattice" + QString::number(i,10));
                med.num = cabinetsettings.value(QString::number(i,10) + "aaa").toInt();
                med.exist = cabinetsettings.value(QString::number(i,10) + "exist").toInt();
                med.cab_num = cabinetsettings.value(QString::number(i,10) + "cab_num").toInt();
                med.lat_num = cabinetsettings.value(QString::number(i,10) + "lat_num").toInt();
                med.name = cabinetsettings.value(QString::number(i,10) + "bbb").toString();
                med.application = cabinetsettings.value(QString::number(i,10) + "ccc").toString();
                med.ShelfLife = cabinetsettings.value(QString::number(i,10) + "ddd").toString();
                med.ProductionDate = cabinetsettings.value(QString::number(i,10) + "eee").toString();
                med.Features = cabinetsettings.value(QString::number(i,10) + "fff").toString();
                medinf[j].removeAt(i);
                medinf[j].insert(i,med); //--每读取一个格子的药品信息放入qlist链表中
                cabinetsettings.endGroup();

                //--恢复所有格子
                if(med.exist == 1)
                {
                    cabinets[med.cab_num].item_add(med.lat_num,0,med.name);
                }
            }
        }
        return 0;
    }
    else
    {
        return 1;
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

    /*写入药柜信息，一个循环写入一个药柜的所有信息*/
    for(int j = 0;j < num; j++)
    {
        //--为每个药柜建立配置文件保存药柜信息：格子数-lattice_num；已用格子数-cab_lattice_num
        QSettings cabinetsettings("cabinet" + QString::number(j,10) + ".ini",QSettings::IniFormat);

        cabinetsettings.beginGroup("cabinet");
        cabinetsettings.setValue("UsedLatticeNum",cab_lattice_num[j]);
        cabinetsettings.endGroup();
        /*先检查药柜是否为空，在写入每个格子的信息*/
            for(int i = 0;i < LatticeNum; i++)
            {
                //--lattice代表格子名称 i表示第几个格子，为每个格子分组
                int seral_num = medinf[j].at(i).num;
                cabinetsettings.beginGroup("lattice" + QString::number(i,10));
                cabinetsettings.setValue(QString::number(i,10) + "aaa",seral_num);
                cabinetsettings.setValue(QString::number(i,10) + "exist",medinf[j].at(i).exist);
                cabinetsettings.setValue(QString::number(i,10) + "cab_num",medinf[j].at(i).cab_num);
                cabinetsettings.setValue(QString::number(i,10) + "lat_num",medinf[j].at(i).lat_num);
                cabinetsettings.setValue(QString::number(i,10) + "bbb",medinf[j].at(i).name);
                cabinetsettings.setValue(QString::number(i,10) + "ccc",medinf[j].at(i).application);
                cabinetsettings.setValue(QString::number(i,10) + "ddd",medinf[j].at(i).Features);
                cabinetsettings.setValue(QString::number(i,10) + "eee",medinf[j].at(i).ProductionDate);
                cabinetsettings.setValue(QString::number(i,10) + "fff",medinf[j].at(i).ShelfLife);
                cabinetsettings.endGroup();
            }
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
            if(medinf[j].at(i).name == str)
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
    }
    else//不存在，做出处理
    {
        MedInf med;
        med.num = row;
        med.cab_num = low;
        med.lat_num = row;
        med.name = str;
        med.exist = 0;
        med.application = "application" + row;
        med.ShelfLife = "ShelfLife" + row;
        med.ProductionDate = "ProductionDate" + row;
        med.Features = "Features" + row;
        show_inf->check_no_exist(med);
    }
    show_inf->show();//show information
    menu_widget->count_close();
}

/**************************
 * 函 数 名：read_showinf
 * 函数功能：绑定扫描型号的草函数，查询扫描的药品是否在药柜
 * 参   数：QByteArray qby--信号传递过来的药品名称
 * 返 回 值：无
 * ***************************/
void MainWidget::read_showinf(MedInf med)
{
    if(med.exist == 0)//药品不存在，插入
    {
        if(medinf[med.cab_num].at(med.lat_num).exist != 1)//
        {
            med.exist = 1;
            medinf[med.cab_num].insert(med.lat_num,med);
            cab_lattice_num[med.cab_num]++;
            cabinets[med.cab_num].item_add(med.lat_num,0,med.name);
            show_inf->btn_close();
            QMessageBox msgBox;
            msgBox.setText("存放完毕，请关好柜门！");
            msgBox.exec();
        }
        else
        {
            QMessageBox msgBox;
            msgBox.setText("请放入空药柜！");
            msgBox.exec();
        }
    }
    else if(med.exist == 1)//药品存在，覆盖
    {
        medinf[med.cab_num].removeAt(med.lat_num);
        medinf[med.cab_num].insert(med.lat_num,med);
        show_inf->btn_close();
        QMessageBox msgBox;
        msgBox.setText("存放完毕，请关好柜门！");
        msgBox.exec();
    }
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

MainWidget::~MainWidget()
{
    delete ui;
    //--写入配置信息
    writeSettings();
}

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QTextStream>

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
    list->insertItem(0, tr("主界面"));list->setSpacing(50);
    list->insertItem(1, tr("菜单"));
    list->insertItem(2, tr("简介"));
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

    //--读取配置信息
    readSettings();

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
    //--设置主药柜的格子数 接口
    cabinets[0].Cabinet_lattice_num_set(lattice_num);

    ui->caseLayout->addWidget(list);  //把list里面的内容加到窗体里面
    ui->caseLayout->addWidget(stack, 0, Qt::AlignHCenter);
    ui->caseLayout->setStretchFactor(list, 1);  //设定为可伸缩的控件，第一个参数是用于指定设置的控件，第二个大于0表示这个控件可伸缩
    ui->caseLayout->setStretchFactor(stack, 5);

    connect(list, SIGNAL(currentRowChanged(int)), stack, SLOT(setCurrentIndex(int)));
    connect(btn_cabinet_add_one,SIGNAL(clicked(bool)),this,SLOT(btn_one()));
    connect(btn_cabinet_add_two,SIGNAL(clicked(bool)),this,SLOT(btn_two()));
    connect(btn_cabinet_add_three,SIGNAL(clicked(bool)),this,SLOT(btn_three()));
    connect(btn_cabinet_add_four,SIGNAL(clicked(bool)),this,SLOT(btn_four()));
    connect(&cabinets[0],SIGNAL(lattice_inf(int)),this,SLOT(cabinets_lattice_zero(int)));
    connect(&cabinets[1],SIGNAL(lattice_inf(int)),this,SLOT(cabinets_lattice_one(int)));
    connect(&cabinets[2],SIGNAL(lattice_inf(int)),this,SLOT(cabinets_lattice_two(int)));
    connect(&cabinets[3],SIGNAL(lattice_inf(int)),this,SLOT(cabinets_lattice_three(int)));
    connect(&cabinets[4],SIGNAL(lattice_inf(int)),this,SLOT(cabinets_lattice_four(int)));
    //--创建药柜文件夹
    mkdir_cabinet();

}

/**************************
 * 函 数 名：menu_set_init
 * 函数功能：stackwidget中菜单的初始化
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::menu_set_init()
{
    label = new QLabel("菜单");
    menu_widget = new QWidget;
    qvbox_menu_layout = new QVBoxLayout();
    qvbox_menu_layout->addWidget(label);
    menu_widget->setLayout(qvbox_menu_layout);
    stack->addWidget(menu_widget);
}

/**************************
 * 函 数 名：cabinets_lattice_zero
 * 函数功能：主药柜的格子添加接口，双击可触发信号，连接此槽函数
 * 参   数：int row--要添加格子的序号
 * 返 回 值：无
 * ***************************/
void MainWidget::cabinets_lattice_zero(int row)
{
    lattice_add(0,row);//--药柜添加第row个格子
}
void MainWidget::cabinets_lattice_one(int row)
{
    lattice_add(1,row);
}
void MainWidget::cabinets_lattice_two(int row)
{
    lattice_add(2,row);
}
void MainWidget::cabinets_lattice_three(int row)
{
    lattice_add(3,row);
}
void MainWidget::cabinets_lattice_four(int row)
{
    lattice_add(4,row);
}

/**************************
 * 函 数 名：mkdir_cabinet
 * 函数功能：建立存放txt文件的上层目录
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::mkdir_cabinet()
{
    /*获取当前路径*/
    QString now_path;
    QDir dir;
    now_path=dir.currentPath();
    QByteArray ba = now_path.toLatin1();
    char *mm = ba.data();
    qDebug("NOW PATH:%s",mm);
    /*判断是否存在药柜目录*/
    path_cabinet = now_path + "/cabinet_dir";
    QDir dir_cabinet(path_cabinet);
    if(dir_cabinet.exists())
    {
        qDebug("exist");
    }
    else
    {
        /*不存在则创建*/
        qDebug("no exist");
        dir_cabinet.mkdir(path_cabinet);
    }
}

/**************************
 * 函 数 名：mkdir_cabinet_txt
 * 函数功能：创建药柜对应的txt文件
 * 参   数：QString name--txt文件名
 *          CabinetPanel *cab--药柜句柄，用于写入或者读取药柜信息
 * 返 回 值：无
 * ***************************/
void MainWidget::mkdir_cabinet_txt(QString name,CabinetPanel *cab)
{
    QString path;
    path = path_cabinet + "/" + name + ".txt";
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
       //-----将数据写入文件
       QByteArray ba = path.toLatin1();
       char *mm = ba.data();
       int LineLen = file.write(mm, strlen(mm)/*sizeof(path)*/);
       //----写入文件失败，将返回 -1
       if (-1 != LineLen)
       {
           //---
//           qDebug() <<"cab->width:"<< cab->width();
       }
   }
   //----关闭文件
   file.close();
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
    num++;
}
void MainWidget::btn_two()
{
    qvbox_two_layout->removeWidget(btn_cabinet_add_two);
    btn_cabinet_add_two->deleteLater();
    qvbox_two_layout->addWidget(&cabinets[2]);
    num++;
}
void MainWidget::btn_three()
{
    qvbox_three_layout->removeWidget(btn_cabinet_add_three);
    btn_cabinet_add_three->deleteLater();
    qvbox_three_layout->addWidget(&cabinets[3]);
    num++;
}
void MainWidget::btn_four()
{
    qvbox_four_layout->removeWidget(btn_cabinet_add_four);
    btn_cabinet_add_four->deleteLater();
    qvbox_four_layout->addWidget(&cabinets[4]);
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
            for(int i = 0;i < cab_lattice_num[j]; i++)
            {
                MedInf med;//--创建临时药品信息类
                cabinetsettings.beginGroup("lattice" + QString::number(i,10));
                med.num = cabinetsettings.value(QString::number(i,10) + "aaa").toInt();
                med.name = cabinetsettings.value(QString::number(i,10) + "bbb").toString();
                med.application = cabinetsettings.value(QString::number(i,10) + "ccc").toString();
                med.ShelfLife = cabinetsettings.value(QString::number(i,10) + "ddd").toString();
                med.ProductionDate = cabinetsettings.value(QString::number(i,10) + "eee").toString();
                med.Features = cabinetsettings.value(QString::number(i,10) + "fff").toString();
                medinf[j].append(med);//--每读取一个格子的药品信息放入qlist链表中
                cabinetsettings.endGroup();

                //--恢复所有格子
                cabinets[j].item_add(med.num,0,med.name);
                qDebug()<<"med.num:"<<med.num;
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
        cabinetsettings.setValue("LatticeNum",lattice_num);
        cabinetsettings.setValue("UsedLatticeNum",cab_lattice_num[j]);
        cabinetsettings.endGroup();
//qDebug()<<"j:"<<j;
        /*先检查药柜是否为空，在写入每个格子的信息*/
        if(!medinf[j].isEmpty())
        {
            for(int i = 0;i < cab_lattice_num[j]; i++)
            {
                //--lattice代表格子名称 i表示第几个格子，为每个格子分组
                int seral_num = medinf[j].at(i).num;
                cabinetsettings.beginGroup("lattice" + QString::number(i,10));
                cabinetsettings.setValue(QString::number(i,10) + "aaa",seral_num);
                cabinetsettings.setValue(QString::number(i,10) + "bbb",medinf[j].at(i).name);
                cabinetsettings.setValue(QString::number(i,10) + "ccc",medinf[j].at(i).application);
                cabinetsettings.setValue(QString::number(i,10) + "ddd",medinf[j].at(i).Features);
                cabinetsettings.setValue(QString::number(i,10) + "eee",medinf[j].at(i).ProductionDate);
                cabinetsettings.setValue(QString::number(i,10) + "fff",medinf[j].at(i).ShelfLife);
                cabinetsettings.endGroup();
//                qDebug()<<"i:"<<i;
            }
        }
        else
        {

        }
    }
}

/**************************
 * 函 数 名：btn_lattice_add()
 * 函数功能：添加格子接口，此函数指定了主药柜
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::lattice_add(int cab,int row)
{
    QString str = "gouji";//--药品名称接口

    //--加入格子内，要显示信息自己控制，此处只显示名字
    cabinets[cab].item_add(row,0,str);
    /* 保存药品的信息在MedInf类中，加入qlist链表中 */
    MedInf med;
    med.num = row;
    med.name = str;
    med.application = "application" + row;
    med.ShelfLife = "ShelfLife" + row;
    med.ProductionDate = "ProductionDate" + row;
    med.Features = "Features" + row;
    //--此处只写入了主药柜的.可根据不同控件对应
    medinf[cab].append(med);

    cab_lattice_num[cab]++;

}

MainWidget::~MainWidget()
{
    delete ui;
    //--写入配置信息
    writeSettings();
}

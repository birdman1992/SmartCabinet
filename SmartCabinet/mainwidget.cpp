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
    cab_lattice_num  = 0;

    //--初始化添加按钮，失败，需要改进
    btn_left = new QPushButton("add");
    btn_right = new QPushButton("add");
    btn_add_lattice = new QPushButton("add lattice");//--添加格子按钮
    btn_left->resize(200,600);
    btn_right->resize(200,600);

    //--设置主药柜的格子数 接口
    cabinets[0].Cabinet_lattice_num_set(lattice_num);

    btn_box = new QVBoxLayout;
    btn_box->addWidget(btn_left);
    btn_box->addWidget(btn_add_lattice);

    ui->caseLayout->addLayout(btn_box);
    ui->caseLayout->addWidget(&cabinets[0]);
    ui->caseLayout->addWidget(btn_right);
//    cabinets->show();

    //--创建药柜文件夹
    mkdir_cabinet();
    //--读取配置信息
    readSettings();

    //connect
    connect(btn_left,SIGNAL(clicked(bool)),this,SLOT(btn_cabinet_add()));
    connect(btn_right,SIGNAL(clicked(bool)),this,SLOT(btn_cabinet_add()));
    connect(btn_add_lattice,SIGNAL(clicked(bool)),this,SLOT(btn_lattice_add()));
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
 * 函 数 名：btn_cabinet_add
 * 函数功能：药柜添加函数，槽函数，绑定添加药柜按钮，最多
 *          增加到5个药柜，奇数药柜在主药柜左边，偶数在右边
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::btn_cabinet_add()
{
    switch (num) {
    case 1:
        ui->caseLayout->addWidget(&cabinets[1]);
        ui->caseLayout->addWidget(&cabinets[0]);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets1",&cabinets[1]);
        cabinets[1].item_add(0,0,tr("冬虫夏草"));
        cabinets[1].Cabinet_lattice_num_set(2);
        num++;
        break;
    case 2:
        ui->caseLayout->addWidget(&cabinets[0]);
        ui->caseLayout->addWidget(&cabinets[2]);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets2",&cabinets[2]);
        cabinets[2].item_add(0,0,"甘草");
        cabinets[2].Cabinet_lattice_num_set(3);
        num++;
        break;
    case 3:
        ui->caseLayout->addWidget(&cabinets[3]);
        ui->caseLayout->addWidget(&cabinets[1]);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets3",&cabinets[3]);
        cabinets[3].item_add(0,0,"灵芝");
        cabinets[3].Cabinet_lattice_num_set(4);
        num++;
        break;
    case 4:
        ui->caseLayout->addWidget(&cabinets[2]);
        ui->caseLayout->addWidget(&cabinets[4]);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets4",&cabinets[4]);
        cabinets[4].item_add(0,0,"人参");
        cabinets[4].Cabinet_lattice_num_set(5);
        num++;
        break;
    default:
        break;
    };
}

/**************************
 * 函 数 名：readSettings
 * 函数功能：读配置文件，读取程序状态数据等，在程序开始时执行
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::readSettings()//读取程序设置
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
                btn_cabinet_add();
            }
        }

        /*读取每个药柜的信息 num为药柜数量*/
        for(int j = 0;j < num; j++)
        {
            QSettings cabinetsettings("cabinet" + QString::number(j,10) + ".ini",QSettings::IniFormat);
            cabinetsettings.beginGroup("cabinet");
            lattice_num = cabinetsettings.value("LatticeNum").toInt();
            cab_lattice_num = cabinetsettings.value("UsedLatticeNum").toInt();
            cabinetsettings.endGroup();

            /*读取每个格子的信息放入qlist；cab_lattice_num为使用的格子数*/
            for(int i = 0;i < cab_lattice_num; i++)
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
                cabinets[j].item_add(i,0,med.name);
            }
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

    /*写入药柜信息，一个循环写入一个药柜的所有信息*/
    for(int j = 0;j < num; j++)
    {
        //--为每个药柜建立配置文件保存药柜信息：格子数-lattice_num；已用格子数-cab_lattice_num
        QSettings cabinetsettings("cabinet" + QString::number(j,10) + ".ini",QSettings::IniFormat);

        cabinetsettings.beginGroup("cabinet");
        cabinetsettings.setValue("LatticeNum",lattice_num);
        cabinetsettings.setValue("UsedLatticeNum",cab_lattice_num);
        cabinetsettings.endGroup();

        /*先检查药柜是否为空，在写入每个格子的信息*/
        if(!medinf[j].isEmpty())
        {
            for(int i = 0;i < cab_lattice_num; i++)
            {
                //--lattice代表格子名称 i表示第几个格子，为每个格子分组
                cabinetsettings.beginGroup("lattice" + QString::number(i,10));
                cabinetsettings.setValue(QString::number(i,10) + "aaa",medinf[j].at(i).num);
                cabinetsettings.setValue(QString::number(i,10) + "bbb",medinf[j].at(i).name);
                cabinetsettings.setValue(QString::number(i,10) + "ccc",medinf[j].at(i).application);
                cabinetsettings.setValue(QString::number(i,10) + "ddd",medinf[j].at(i).Features);
                cabinetsettings.setValue(QString::number(i,10) + "eee",medinf[j].at(i).ProductionDate);
                cabinetsettings.setValue(QString::number(i,10) + "fff",medinf[j].at(i).ShelfLife);
                cabinetsettings.endGroup();
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
void MainWidget::btn_lattice_add()
{
    QString str = "gouji";//--药品名称接口
    if(cab_lattice_num < lattice_num)//--格子满了不予以放入
    {
        //--加入格子内，要显示信息自己控制，此处只显示名字
        cabinets[0].item_add(cab_lattice_num,0,str);
        /* 保存药品的信息在MedInf类中，加入qlist链表中 */
        MedInf med;
        med.num = cab_lattice_num;
        med.name = str;
        med.application = "application" + cab_lattice_num;
        med.ShelfLife = "ShelfLife" + cab_lattice_num;
        med.ProductionDate = "ProductionDate" + cab_lattice_num;
        med.Features = "Features" + cab_lattice_num;
        //--此处只写入了主药柜的.可根据不同控件对应
        medinf[0].append(med);

        cab_lattice_num++;
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("药柜格子不足");
        msgBox.exec();
    }
}

MainWidget::~MainWidget()
{
    delete ui;
    //--写入配置信息
    writeSettings();
}

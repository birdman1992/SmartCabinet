#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QtDebug>
#include <QMessageBox>
#include <QDir>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    num = 1;//--初始一个药柜

    //--初始化添加按钮，失败，需要改进
    btn_left = new QPushButton("add");
    btn_right = new QPushButton("add");
    btn_left->resize(200,600);
    btn_right->resize(200,600);

    //--初始化主药柜，必须在添加时初始化，如果先初始化而不加入布局，会出现bug
    cabinets = new CabinetPanel(this);
    cabinets->item_add(0,0,"板蓝根");

    ui->caseLayout->addWidget(btn_left);
    ui->caseLayout->addWidget(cabinets);
    ui->caseLayout->addWidget(btn_right);
    cabinets->show();

    //--创建药柜文件夹
    mkdir_cabinet();
    //--读取配置信息
    readSettings();

    //connect
    connect(btn_left,SIGNAL(clicked(bool)),this,SLOT(btn_cabinet_add()));
    connect(btn_right,SIGNAL(clicked(bool)),this,SLOT(btn_cabinet_add()));
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
        cabinets1 = new CabinetPanel(this);
        ui->caseLayout->addWidget(cabinets1);
        ui->caseLayout->addWidget(cabinets);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets1",cabinets1);
        cabinets1->item_add(0,0,tr("冬虫夏草"));
        num++;
        break;
    case 2:
        cabinets2 = new CabinetPanel(this);
        ui->caseLayout->addWidget(cabinets);
        ui->caseLayout->addWidget(cabinets2);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets2",cabinets2);
        cabinets2->item_add(0,0,"甘草");
        num++;
        break;
    case 3:
        cabinets3 = new CabinetPanel(this);
        ui->caseLayout->addWidget(cabinets3);
        ui->caseLayout->addWidget(cabinets1);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets3",cabinets3);
        cabinets3->item_add(0,0,"灵芝");
        num++;
        break;
    case 4:
        cabinets4 = new CabinetPanel(this);
        ui->caseLayout->addWidget(cabinets2);
        ui->caseLayout->addWidget(cabinets4);
        ui->caseLayout->addWidget(btn_right);
        mkdir_cabinet_txt("cabinets4",cabinets4);
        cabinets4->item_add(0,0,"人参");
        num++;
        break;
    default:
        break;
    };
    cabinets->show();
}
/**************************
 * 函 数 名：readSettings
 * 函数功能：读配置文件，读取程序状态数据等，在程序开始时执行
 * 参   数：无
 * 返 回 值：无
 * ***************************/
void MainWidget::readSettings()//读取程序设置
{
    QSettings setting("Option.ini",QSettings::IniFormat);
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
                btn_cabinet_add();
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
    QSettings settings("Option.ini",QSettings::IniFormat);
    settings.setValue("num",num);
    settings.setValue("path",path_cabinet);
    /*药柜分组*/
    settings.beginGroup("cabinet1");
    settings.setValue("num",num);
    settings.endGroup();
    settings.beginGroup("cabinet2");
    settings.setValue("num",num);
    settings.endGroup();
    settings.beginGroup("cabinet3");
    settings.setValue("num",num);
    settings.endGroup();
    settings.beginGroup("cabinet4");
    settings.setValue("num",num);
    settings.endGroup();

}

MainWidget::~MainWidget()
{
    delete ui;
    //--写入配置信息
    writeSettings();
}

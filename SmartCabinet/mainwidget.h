#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QStyleFactory>
#include <QPaintEvent>
#include <QPainter>
#include "Cabinet/cabinetpanel.h"
#include <QPushButton>
#include <QSettings>
#include "Device/controldevice.h"
#include "medinf.h"
#include <QList>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>

#include "cabinetconfig.h"
#include "cabinetserver.h"
#include "tcpserver.h"
#include "Widgets/standbywidget.h"
#include "Widgets/userwidget.h"
#include "Widgets/cabinetset.h"
#include "Widgets/coderkeyboard.h"
#include "Widgets/cabinetservice.h"
#include "Widgets/checktable.h"
#include "Widgets/goodsapply.h"
#include "Widgets/dayreport.h"
#include "Widgets/aiomachine.h"
#include "Cabinet/cabinetwidget.h"
#include "MxAuthor/authormanager.h"
#include "funcs/routerepair.h"

#include "Menu/setmenu.h"
#include "ShowInf/showinf.h"
#include "PrimaryUser/primaryuser.h"
#include "PrimaryUser/userinf.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

public slots:
    void cabinetClear();
    void globalTouch();

protected:

private:
    Ui::MainWidget *ui;
    CabinetConfig* cabinetConf;
#ifdef TCP_API
    tcpServer* cabServer;
#else
    CabinetServer* cabServer;
#endif

    UserWidget* win_user_manage;//用户管理窗口
    StandbyWidget* win_standby;//待机窗口
    CabinetSet* win_cabinet_set;//智能柜组合设置窗口
    CabinetWidget* win_cabinet;//智能柜窗口
    coderKeyboard* win_coder_keyboard;//条码输入窗口
    CabinetService* win_cab_service;//服务窗口
    CheckTable* win_check_table;//盘点表格窗口
    GoodsApply* win_goods_apply;//请货窗口
    DayReport* win_day_report;//日清单窗口
    AIOMachine* win_aio;
    RouteRepair* routeRepair;//路由修复
    ControlDevice* ctrlUi;

    QStackedWidget *stack;             //堆栈窗体
    QListWidget *list;                 //列表框

    void init_xiangang();
    void init_huangpo();
    void connect_master();
    void connect_new_api();
    void paintEvent(QPaintEvent *);
};

#endif // MAINWIDGET_H

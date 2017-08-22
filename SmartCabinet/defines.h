#ifndef DEFINES_H
#define DEFINES_H

#define CONF_USER "/home/config/cabinet.ini"
#define CONF_CABINET "/home/config/cabinet.ini"
#define CAB_CACHE "/home/config/cache.ini"

/*模拟无服务器状态*/
//#define NO_SERVER

#define INDEX_STANDBY 2
#define INDEX_USER_MANAGE 3
#define INDEX_CAB_SET 4
#define INDEX_CAB_SHOW 5
#define INDEX_CAB_SERVICE 6

//#define VICE_CAB_CASE_NUM 6 //副柜格子数
//#define Main_CAB_CASE_NUM (VICE_CAB_CASE_NUM-2) //主柜格子数
#define CAB_CASE_0_NUM 8   //副柜
#define CAB_CASE_1_NUM 6   //主柜
#define CAB_CASE_2_NUM 8

#define MSG_EMPTY ""
#define MSG_SCAN_LIST "请扫描送货单条码"
#define MSG_LIST_ERROR "无效的送货单"
#define MSG_STORE "请扫描待存放物品条形码"
#define MSG_STORE_SELECT "请选择绑定位置"
#define MSG_STORE_SELECT_REPEAT "选择的位置被占用 请重新选择"
#define MSG_FETCH "请选择要取出的物品 柜门打开后请扫描条形码取出"
#define MSG_FETCH_SCAN "请扫描条形码取出物品 取用完毕请点击此处并关闭柜门"
#define MSG_FETCH_EMPTY "没有库存了 请关好柜门 点击此处退出"
#define MSG_CHECK  "请点击柜格开始盘点"
#define MSG_REFUND  "点击柜格扫码退货"
#define MSG_OFFLINE  "当前为离线状态，请点击 切换 按钮批量取货"
#define MSG_REBIND_SCAN     "点击柜格：开门\n扫描条码：选择需要变更的物品"
#define MSG_REBIND_SELECT   "请选择物品重新绑定的柜格"


#endif // DEFINES_H

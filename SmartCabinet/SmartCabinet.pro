#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T10:58:40
#
#-------------------------------------------------

QT       += core gui network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include($$PWD/log/log.pri)

TARGET = SmartCabinet
TEMPLATE = app
target.path = /home
INSTALLS += target

LIBS += -lpthread

if(contains(DEFINES,PC)){
LIBS += -L$$PWD/libs/PC/libusb -lusb-1.0
LIBS += -L$$PWD/Crypto/x64 -lcrypto
}
if(contains(DEFINES,MC)){
LIBS += -L$$PWD/libs/MC/libusb -lusb-1.0
LIBS += -L$$PWD/libs/MC/Crypto -lcrypto
}
if(contains(DEFINES,MY)){
LIBS += -L$$PWD/libs/MY/libusb -lusb-1.0
LIBS += -L$$PWD/libs/MY/Crypto -lcrypto
}

SOURCES += main.cpp\
        mainwidget.cpp \
    Cabinet/cabinetpanel.cpp \
    Device/Qextserial/qextserialport.cpp \
    Device/Qextserial/qextserialport_unix.cpp \
    Device/Hid/qhid.cpp \
    Device/controldevice.cpp \
    Device/devicesimulate.cpp \
    medinf.cpp \
    Widgets/standbywidget.cpp \
    Widgets/userwidget.cpp \
    cabinetconfig.cpp \
    Structs/userinfo.cpp \
    Widgets/cabinetset.cpp \
    Structs/cabinetinfo.cpp \
    Menu/setmenu.cpp \
    Countdown/countdown.cpp \
    ShowInf/showinf.cpp \
    PrimaryUser/primaryuser.cpp \
    PrimaryUser/userinf.cpp \
    Cabinet/cabinet.cpp \
    Cabinet/cabinetwidget.cpp \
    Structs/caseaddress.cpp \
    cabinetserver.cpp \
    Json/cJSON.c \
    Structs/goodslist.cpp \
    Widgets/cabinetaccess.cpp \
    Widgets/numkeyboard.cpp \
    Device/SerialPort/qserialport.cpp \
    Widgets/coderkeyboard.cpp \
    Widgets/cabinetservice.cpp \
    Device/Network/qnetinterface.cpp \
    inputcontex/myinputpanelcontext.cpp \
    inputcontex/keyboard.cpp \
    Device/voiceplayer.cpp \
    Widgets/cabinetlistview.cpp \
    Widgets/cabinetlistitem.cpp \
    funcs/chineseletterhelper.cpp \
    Widgets/cabinetcheck.cpp \
    Widgets/cabinetcheckitem.cpp \
    Widgets/cabinetstorelist.cpp \
    Widgets/cabinetstorelistitem.cpp \
    Structs/goodscar.cpp \
    Widgets/cabinetrefund.cpp \
    Widgets/cabinetrefunditem.cpp \
    Widgets/cabinetctrlconfig.cpp \
    Widgets/casepanel.cpp \
    globalapp.cpp \
    Device/Network/networkset.cpp \
    Device/QDeviceWatcher/qdevicewatcher.cpp \
    Widgets/checktable.cpp \
    Structs/goodscheckinfo.cpp \
    Widgets/goodsapply.cpp \
    Structs/goodssearchinfo.cpp \
    Widgets/keyboad.cpp \
    Widgets/checkwarning.cpp \
    Widgets/dayreport.cpp \
    Structs/dayreportinfo.cpp \
    tcpserver.cpp \
    Crypto/qaes.cpp \
    manager/usermanager.cpp \
    manager/cabinetmanager.cpp \
    manager/goodsmanager.cpp \
    Device/Hid/hid-libusb.c \
    MxAuthor/authorencrypt.cpp \
    MxAuthor/authormanager.cpp \
    manager/lockmanager.cpp \
    funcs/servertest.cpp \
    funcs/routerepair.cpp \
    Structs/versioninfo.cpp \
    logreport.cpp \
    funcs/operationrecorder.cpp \
    Widgets/stroebyitem.cpp \
    manager/storelistmanager.cpp \
    aio/aiomachine.cpp \
    aio/aiobutton.cpp \
    aio/aiooverview.cpp \
    funcs/screenshot.cpp \
    gpio/gpioapi.cpp \
    Device/ledctrl.cpp \
    cabinettcp.cpp \
    Device/tempdev.cpp


HEADERS  += mainwidget.h \
    Cabinet/cabinetpanel.h \
    Device/Qextserial/qextserialport.h \
    Device/Qextserial/qextserialport_p.h \
    Device/Qextserial/qextserialport_global.h \
    Device/Hid/qhid.h \
    Device/controldevice.h \
    Device/devicesimulate.h \
    medinf.h \
    Widgets/standbywidget.h \
    Widgets/userwidget.h \
    cabinetconfig.h \
    Structs/userinfo.h \
    defines.h \
    Widgets/cabinetset.h \
    Structs/cabinetinfo.h \
    Menu/setmenu.h \
    Countdown/countdown.h \
    ShowInf/showinf.h \
    PrimaryUser/primaryuser.h \
    PrimaryUser/userinf.h \
    Cabinet/cabinet.h \
    Cabinet/cabinetwidget.h \
    Structs/caseaddress.h \
    beautifului.h \
    cabinetserver.h \
    Json/cJSON.h \
    Structs/goodslist.h \
    Widgets/cabinetaccess.h \
    Widgets/numkeyboard.h \
    Device/SerialPort/qserialport.h \
    Widgets/coderkeyboard.h \
    Widgets/cabinetservice.h \
    Device/Network/qnetinterface.h \
    inputcontex/myinputpanelcontext.h \
    inputcontex/keyboard.h \
    Device/voiceplayer.h \
    Widgets/cabinetlistview.h \
    Widgets/cabinetlistitem.h \
    funcs/chineseletterhelper.h \
    Widgets/cabinetcheck.h \
    Widgets/cabinetcheckitem.h \
    Widgets/cabinetstorelist.h \
    Widgets/cabinetstorelistitem.h \
    Structs/goodscar.h \
    Widgets/cabinetrefund.h \
    Widgets/cabinetrefunditem.h \
    Widgets/cabinetctrlconfig.h \
    Widgets/casepanel.h \
    globalapp.h \
    Device/Network/networkset.h \
    Device/QDeviceWatcher/qdevicewatcher.h \
    Widgets/checktable.h \
    Structs/goodscheckinfo.h \
    Structs/goodssearchinfo.h \
    Widgets/checkwarning.h \
    Widgets/dayreport.h \
    Structs/dayreportinfo.h \
    tcpserver.h \
    Crypto/opensslconf.h \
    Crypto/aes.h \
    Crypto/qaes.h \
    manager/usermanager.h \
    manager/cabinetmanager.h \
    manager/goodsmanager.h \
    Widgets/keyboad.h \
    Widgets/goodsapply.h \
    Device/Hid/libusb.h \
    Device/Hid/hidapi.h \
    MxAuthor/authorencrypt.h \
    MxAuthor/authormanager.h \
    manager/lockmanager.h \
    funcs/servertest.h \
    funcs/routerepair.h \
    Structs/versioninfo.h \
    logreport.h \
    funcs/operationrecorder.h \
    Widgets/stroebyitem.h \
    manager/storelistmanager.h \
    aio/aiomachine.h \
    aio/aiobutton.h \
    aio/aiooverview.h \
    funcs/screenshot.h \
    gpio/gpioapi.h \
    Device/ledctrl.h \
    cabinettcp.h \
    Device/tempdev.h

FORMS    += mainwidget.ui \
    Cabinet/cabinetpanel.ui \
    Device/devicesimulate.ui \
    Widgets/standbywidget.ui \
    Widgets/userwidget.ui \
    Widgets/cabinetset.ui \
    Menu/setmenu.ui \
    Countdown/countdown.ui \
    ShowInf/showinf.ui \
    PrimaryUser/primaryuser.ui \
    Cabinet/cabinet.ui \
    Cabinet/cabinetwidget.ui \
    Widgets/cabinetaccess.ui \
    Widgets/numkeyboard.ui \
    Widgets/coderkeyboard.ui \
    Widgets/cabinetservice.ui \
    inputcontex/keyboard.ui \
    Widgets/cabinetlistview.ui \
    Widgets/cabinetlistitem.ui \
    Widgets/cabinetcheck.ui \
    Widgets/cabinetcheckitem.ui \
    Widgets/cabinetstorelist.ui \
    Widgets/cabinetstorelistitem.ui \
    Widgets/cabinetrefund.ui \
    Widgets/cabinetrefunditem.ui \
    Widgets/cabinetctrlconfig.ui \
    Widgets/casepanel.ui \
    Device/Network/networkset.ui \
    Widgets/checktable.ui \
    Widgets/checkwarning.ui \
    Widgets/dayreport.ui \
    Widgets/keyboad.ui \
    Widgets/goodsapply.ui \
    Widgets/stroebyitem.ui \
    aio/aiomachine.ui

RESOURCES += \
    image.qrc

#DISTFILES += \
#    Jsoncpp/sconscript \
#    Crypto/libcrypto.so.1.1 \
#    Crypto/libcrypto.so


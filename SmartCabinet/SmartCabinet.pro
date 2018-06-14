#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T10:58:40
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartCabinet
TEMPLATE = app
target.path = /home
INSTALLS += target

LIBS += -L$$PWD/Crypto/x64 -lcrypto

SOURCES += main.cpp\
        mainwidget.cpp \
    Cabinet/cabinetpanel.cpp \
    Device/Qextserial/qextserialport.cpp \
    Device/Qextserial/qextserialport_unix.cpp \
    Device/Hid/qhid.cpp \
    Device/controldevice.cpp \
    Device/Hid/hid.c \
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
    tcpserver.cpp \
    Crypto/qaes.cpp

HEADERS  += mainwidget.h \
    Cabinet/cabinetpanel.h \
    Device/Qextserial/qextserialport.h \
    Device/Qextserial/qextserialport_p.h \
    Device/Qextserial/qextserialport_global.h \
    Device/Hid/qhid.h \
    Device/controldevice.h \
    Device/Hid/hid.h \
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
    tcpserver.h \
    Crypto/opensslconf.h \
    Crypto/aes.h \
    Crypto/qaes.h

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
    Widgets/checktable.ui

RESOURCES += \
    image.qrc

DISTFILES += \
    Jsoncpp/sconscript \
    Crypto/libcrypto.so.1.1 \
    Crypto/libcrypto.so


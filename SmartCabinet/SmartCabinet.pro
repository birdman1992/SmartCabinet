#-------------------------------------------------
#
# Project created by QtCreator 2017-04-19T10:58:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartCabinet
TEMPLATE = app


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
    Cabinet/cabinet.cpp


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
    beautifului.h


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
    Cabinet/cabinet.ui

RESOURCES += \
    image.qrc

DISTFILES +=

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
    Cabinet/cabinetpanel.cpp

HEADERS  += mainwidget.h \
    Cabinet/cabinetpanel.h

FORMS    += mainwidget.ui \
    Cabinet/cabinetpanel.ui

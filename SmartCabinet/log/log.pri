INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
HEADERS += \
	$$PWD/Log.h

SOURCES += \
	$$PWD/Log.cpp

include($$PWD/log4qt/src/log4qt/log4qt.pri)

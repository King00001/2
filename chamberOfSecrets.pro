TARGET = chamberOfSecrets
TEMPLATE = app

CONFIG += C++11 console
QT -= gui
QT       += sql
QT += network

RC_FILE = appIco.rc

SOURCES += main.cpp \
    mainhandler.cpp \
    Controller/usercontroller.cpp \
    Controller/controller.cpp \
    Controller/themecontroller.cpp \
    Controller/portcontroller.cpp \
    ../../NDBPool-master/NDBPool/src/ndbpool.cpp \
    ../../NDBPool-master/NDBPool/src/ndbpool_p.cpp \
    globaldata.cpp \
    ../../common/ndbpoolhelper.cpp \
    webtcpserver.cpp \
    webworker.cpp \
    service.cpp \
    tcpsocketmanager.cpp \
    tcpsocketmanagerdevice.cpp \
    ../../common/src/fcsmanager.cpp \
    devicetcpserver.cpp \
    deviceworker.cpp \
    websocketmanager.cpp \
    ../../common/src/property.cpp \
    Controller/filecontroller.cpp

HEADERS += \
    mainhandler.h \
    type.h \
    type.h \
    Controller/usercontroller.h \
    Controller/controller.h \
    Controller/themecontroller.h \
    Controller/portcontroller.h \
    ../../NDBPool-master/NDBPool/inc/ndbpool.h \
    ../../NDBPool-master/NDBPool/inc/ndbpool_global.h \
    ../../NDBPool-master/NDBPool/inc/ndbpool_p.h \
    globaldata.h \
    ../../common/ndbpoolhelper.h \
    webtcpserver.h \
    webworker.h \
    service.h \
    tcpsocketmanager.h \
    tcpsocketmanagerdevice.h \
    ../common/include/typedef.h \
    devicetcpserver.h \
    deviceworker.h \
    websocketmanager.h \
    ../../common/include/property.h \
    Controller/filecontroller.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../tufao-1.x/lib/ -llibtufao1.dll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../tufao-1.x/lib/ -llibtufao1d.dll

INCLUDEPATH += $$PWD/../../../tufao-1.x/include
DEPENDPATH += $$PWD/../../../tufao-1.x/include

INCLUDEPATH += $$PWD/../../common/include
INCLUDEPATH += $$PWD/../../common

INCLUDEPATH += $$PWD/../common/include
INCLUDEPATH += $$PWD/../common

INCLUDEPATH += $$PWD/Controller

INCLUDEPATH += $$PWD/../../NDBPool-master/NDBPool/inc

INCLUDEPATH += $$PWD/../../NDBPool-master/bin
DEPENDPATH += $$PWD/../../NDBPool-master/bin

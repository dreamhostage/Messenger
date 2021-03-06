#-------------------------------------------------
#
# Project created by QtCreator 2019-06-24T19:56:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RemoteInterceptionServerGui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += "C:/libs/boost"
LIBS += "C:/libs/boost/stage/lib/libboost_date_time-vc142-mt-gd-x64-1_70.lib"
LIBS += "C:/libs/boost/stage/lib/libboost_regex-vc142-mt-gd-x64-1_70.lib"

LIBS += "C:/libs/boost/stage/lib/libboost_date_time-vc142-mt-x64-1_70.lib"
LIBS += "C:/libs/boost/stage/lib/libboost_regex-vc142-mt-x64-1_70.lib"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        message.cpp

HEADERS += \
        mainwindow.h \
        message.h

FORMS += \
        mainwindow.ui \
        message.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target




win32: LIBS += -L$$PWD/../x64/Release/ -lInterceptingServerLibrary

INCLUDEPATH += $$PWD/../InterceptingServerLibrary
DEPENDPATH += $$PWD/../InterceptingServerLibrary

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../x64/Release/InterceptingServerLibrary.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../x64/Release/libInterceptingServerLibrary.a

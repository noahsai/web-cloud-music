#-------------------------------------------------
#
# Project created by QtCreator 2017-12-07T19:24:15
#
#-------------------------------------------------

QT       += core gui   x11extras webenginewidgets webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = webmusic
TEMPLATE = app

LIBS += -lX11 -lXext


SOURCES += main.cpp\
        webmusic.cpp \
    lrcdesktop.cpp \
    cache.cpp \
    myqwebpage.cpp \
    mywebview.cpp

HEADERS  += webmusic.h \
    lrcdesktop.h \
    cache.h \
    myqwebpage.h \
    mywebview.h

FORMS    += webmusic.ui \
    lrcdesktop.ui \
    cache.ui

RESOURCES += \
    icon.qrc

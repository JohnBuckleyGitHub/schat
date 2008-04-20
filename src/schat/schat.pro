# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE = app
TARGET = 
DEPENDPATH  += . \
               ../common \
               ../schatd               
INCLUDEPATH += . \
               ../common \
               ../schatd
QT += network
#CONFIG += console
#DEFINES += SCHAT_DEBUG SCHAT_CLIENT
DEFINES += SCHAT_CLIENT

RESOURCES += ../../data/schat.qrc

HEADERS += \
    aboutdialog.h \
    chatbrowser.h \
    clientsocket.h \
    directchannel.h \
    directchannelserver.h \
    mainchannel.h \
    profile.h \
    protocol.h \
    schatwindow.h \
    server.h \
    serversocket.h \
    settings.h \
    settingsdialog.h \
    tab.h \
    version.h \
    welcomedialog.h

SOURCES += \
    aboutdialog.cpp \
    chatbrowser.cpp \
    clientsocket.cpp \
    directchannel.cpp \
    directchannelserver.cpp \
    main.cpp \
    mainchannel.cpp \
    profile.cpp \
    schatwindow.cpp \
    server.cpp \
    serversocket.cpp \
    settings.cpp \
    settingsdialog.cpp \
    tab.cpp \
    welcomedialog.cpp

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/schat/debug/rcc
    MOC_DIR = ../../tmp/schat/debug/moc
    OBJECTS_DIR = ../../tmp/schat/debug/obj
    DESTDIR = ../../out/debug
}

CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/schat/release/rcc
    MOC_DIR = ../../tmp/schat/release/moc
    OBJECTS_DIR = ../../tmp/schat/release/obj
    DESTDIR = ../../out/release
}

FORMS = welcomedialog.ui
win32:RC_FILE = schat.rc
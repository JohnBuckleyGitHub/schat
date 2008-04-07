# $Id$
# Simple Chat
# Copyright � 2008 IMPOMEZIA (http://impomezia.net.ru)

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

# Input
HEADERS += tab.h \
           directchannel.h \
           clientsocket.h \
           mainchannel.h \
           schatwindow.h \
           protocol.h \
           server.h \
           serversocket.h \
           welcomedialog.h \
           version.h
SOURCES += tab.cpp \
           directchannel.cpp \
           clientsocket.cpp \
           mainchannel.cpp \
           main.cpp \
           schatwindow.cpp \
           server.cpp \
           serversocket.cpp \
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
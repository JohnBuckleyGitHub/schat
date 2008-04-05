# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE = app
QT += network
CONFIG += console
HEADERS = protocol.h \
          serversocket.h \
          server.h \
          version.h
SOURCES = serversocket.cpp \
          server.cpp \
          main.cpp
    
DEPENDPATH  += . \
               ../common
                
INCLUDEPATH += . \
               ../common

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/schatd/debug/rcc
    MOC_DIR = ../../tmp/schatd/debug/moc
    OBJECTS_DIR = ../../tmp/schatd/debug/obj
    DESTDIR = ../../out/debug
}
CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/schatd/release/rcc
    MOC_DIR = ../../tmp/schatd/release/moc
    OBJECTS_DIR = ../../tmp/schatd/release/obj
    DESTDIR = ../../out/release
}
win32:RC_FILE = schatd.rc
#DEFINES += SCHAT_DEBUG
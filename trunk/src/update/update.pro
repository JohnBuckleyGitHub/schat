# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE = app
TARGET = schat-update 
QT = core network
CONFIG += console

HEADERS += downloadmanager.h
SOURCES += downloadmanager.cpp main.cpp

DEPENDPATH  += . \
               ../common
                
INCLUDEPATH += . \
               ../common

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/update/debug/rcc
    MOC_DIR = ../../tmp/update/debug/moc
    OBJECTS_DIR = ../../tmp/update/debug/obj
    DESTDIR = ../../out/debug
}
CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/update/release/rcc
    MOC_DIR = ../../tmp/update/release/moc
    OBJECTS_DIR = ../../tmp/update/release/obj
    DESTDIR = ../../out/release
}
win32:RC_FILE = update.rc
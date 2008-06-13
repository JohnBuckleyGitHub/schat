# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE = app
TARGET = schat-update 
QT += network
CONFIG += console
RESOURCES += ../../data/update.qrc

HEADERS += \
    download.h \
    install.h \
    update.h \
    updatexmlreader.h \
    version.h
SOURCES += \
    download.cpp \
    install.cpp \
    main.cpp \
    update.cpp \
    updatexmlreader.cpp \

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
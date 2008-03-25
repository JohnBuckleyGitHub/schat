# $Id$
# Simple Chat
# Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)

TEMPLATE = app
QT += network
CONFIG += console
HEADERS = protocol.h \
    serversocket.h \
    server.h
SOURCES = serversocket.cpp \
    server.cpp \
    main.cpp
    
DEPENDPATH  += . \
               ..\common
                
INCLUDEPATH += . \
               ..\common

CONFIG(debug, debug|release) { 
    RCC_DIR = ../../tmp/schatd/debug/rcc
    UI_DIR = ../../tmp/schatd/debug/ui
    MOC_DIR = ../../tmp/schatd/debug/moc
    OBJECTS_DIR = ../../tmp/schatd/debug/obj
    DESTDIR = ../../out/debug
}
CONFIG(release, debug|release) { 
    RCC_DIR = ../../tmp/schatd/release/rcc
    UI_DIR = ../../tmp/schatd/release/ui
    MOC_DIR = ../../tmp/schatd/release/moc
    OBJECTS_DIR = ../../tmp/schatd/release/obj
    DESTDIR = ../../out/release
}
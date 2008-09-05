# $Id$
# IMPOMEZIA Simple Chat
# Copyright � 2008 IMPOMEZIA (http://impomezia.net.ru)
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = app
TARGET = 
DEPENDPATH  += . \
               ../common \
                            
INCLUDEPATH += . \
               ../common \
               
QT += network

RESOURCES += ../../data/schat.qrc

HEADERS += \
    aboutdialog.h \
    abstractprofile.h \
    abstractsettings.h \
    abstracttab.h \
    channellog.h \
    chatbrowser.h \
    clientservice.h \
    emoticonmovie.h \
    icondefreader.h \
    mainchannel.h \
    network.h \
    networkreader.h \
    networkwidget.h \
    profilewidget.h \
    protocol.h \
    schatwindow.h \
    settings.h \
    settingsdialog.h \
    tab.h \
    version.h \
    welcomedialog.h \
    widget/emoticonselector.h \
    widget/inputwidget.h \
    widget/sendwidget.h \
    emoticonsreader.h \

SOURCES += \
    aboutdialog.cpp \
    abstractprofile.cpp \
    abstractsettings.cpp \
    abstracttab.cpp \
    channellog.cpp \
    chatbrowser.cpp \
    clientservice.cpp \
    emoticonmovie.cpp \
    icondefreader.cpp \
    main.cpp \
    mainchannel.cpp \
    network.cpp \
    networkreader.cpp \
    networkwidget.cpp \
    profilewidget.cpp \
    schatwindow.cpp \
    settings.cpp \
    settingsdialog.cpp \
    tab.cpp \
    welcomedialog.cpp \
    widget/emoticonselector.cpp \
    widget/inputwidget.cpp \
    widget/sendwidget.cpp \
    emoticonsreader.cpp \

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

win32 {
    RC_FILE = schat.rc
    HEADERS += win32/updatenotify.h
    SOURCES += win32/updatenotify.cpp
    DEFINES += SCHAT_UPDATE
}

INCLUDEPATH += ../3rdparty/singleapplication/include
SINGLEAPPLICATION_LIBCONFIG = staticlib
win32-msvc*:LIBS += ../3rdparty/singleapplication/bin/singleapplication.lib
else:LIBS += -L ../3rdparty/singleapplication/bin -lsingleapplication

include(schat.pri)

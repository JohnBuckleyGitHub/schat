# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_CONSOLE    = 0
SCHAT_DEBUG      = 0
SCHAT_SINGLE_APP = 1
SCHAT_RESOURCES  = 1
SCHAT_RC_FILE    = 1
SCHAT_STATIC     = 0

DEFINES += SCHAT_CLIENT

QT += network
TEMPLATE = app

HEADERS += \
    aboutdialog.h \
    abstractprofile.h \
    abstractsettings.h \
    abstractsettingsdialog.h \
    abstracttab.h \
    channellog.h \
    chatbrowser.h \
    clientservice.h \
    emoticonmovie.h \
    emoticonsreader.h \
    icondefreader.h \
    mainchannel.h \
    network.h \
    networkreader.h \
    profilewidget.h \
    protocol.h \
    schatwindow.h \
    settings.h \
    settingsdialog.h \
    tab.h \
    trayicon.h \
    update/downloadmanager.h \
    update/update.h \
    update/updatexmlreader.h \
    version.h \
    welcomedialog.h \
    widget/emoticonselector.h \
    widget/inputwidget.h \
    widget/networkwidget.h \
    widget/sendwidget.h \
    widget/userview.h \

SOURCES += \
    aboutdialog.cpp \
    abstractprofile.cpp \
    abstractsettings.cpp \
    abstractsettingsdialog.cpp \
    abstracttab.cpp \
    channellog.cpp \
    chatbrowser.cpp \
    clientservice.cpp \
    emoticonmovie.cpp \
    emoticonsreader.cpp \
    icondefreader.cpp \
    main.cpp \
    mainchannel.cpp \
    network.cpp \
    networkreader.cpp \
    profilewidget.cpp \
    schatwindow.cpp \
    settings.cpp \
    settingsdialog.cpp \
    tab.cpp \
    trayicon.cpp \
    update/downloadmanager.cpp \
    update/update.cpp \
    update/updatexmlreader.cpp \
    welcomedialog.cpp \
    widget/emoticonselector.cpp \
    widget/inputwidget.cpp \
    widget/networkwidget.cpp \
    widget/sendwidget.cpp \
    widget/userview.cpp \

win32 {
    HEADERS += update/verifythread.h
    SOURCES += update/verifythread.cpp
}
else {
    DEFINES += SCHAT_NO_UPDATE
}

contains( SCHAT_STATIC, 1 ) {
    QTPLUGIN += qgif
    DEFINES  += SCHAT_STATIC
}

TRANSLATIONS += ../../data/translations/schat_ru.ts
CODECFORTR = UTF-8

include(../common/common.pri)

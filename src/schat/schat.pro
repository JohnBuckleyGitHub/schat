# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2009 IMPOMEZIA <schat@impomezia.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.

SCHAT_CONSOLE    = 0
SCHAT_WEBKIT     = 1
SCHAT_DEBUG      = 0
SCHAT_SINGLE_APP = 0
SCHAT_RESOURCES  = 1
SCHAT_RC_FILE    = 1
SCHAT_STATIC     = 0
SCHAT_BENCHMARK  = 0

DEFINES += SCHAT_CLIENT

QT += network webkit
TEMPLATE = app

HEADERS += \
    aboutdialog.h \
    abstractprofile.h \
    abstractsettings.h \
    abstractsettingsdialog.h \
    abstracttab.h \
    channellog.h \
    chatwindow/chatview.h \
    chatwindow/chatview_p.h \
    clientservice.h \
    emoticons/emoticons.h \
    emoticons/emoticonselector.h \
    emoticons/emoticonsprovider.h \
    emoticons/emoticonstheme.h \
    emoticons/providers/adium/adium_emoticons.h \
    emoticons/providers/kde/kde_emoticons.h \
    emoticons/providers/pidgin/pidgin_emoticons.h \
    emoticons/providers/xmpp/xmpp_emoticons.h \
    idle/idle.h \
    mainchannel.h \
    network.h \
    networkreader.h \
    popup/popupmanager.h \
    popup/popupmanager_p.h \
    popup/popupwindow.h \
    profilewidget.h \
    protocol.h \
    schatwindow.h \
    schatwindow_p.h \
    settings.h \
    settingsdialog.h \
    singleapplication.h \
    tab.h \
    trayicon.h \
    update/downloadmanager.h \
    update/update.h \
    update/updatewidget.h \
    updatexmlreader.h \
    version.h \
    welcomedialog.h \
    widget/inputwidget.h \
    widget/networkwidget.h \
    widget/sendwidget.h \
    widget/soundwidget.h \
    widget/userview.h \
    widget/userview_p.h \

SOURCES += \
    aboutdialog.cpp \
    abstractprofile.cpp \
    abstractsettings.cpp \
    abstractsettingsdialog.cpp \
    abstracttab.cpp \
    channellog.cpp \
    chatwindow/chatview.cpp \
    clientservice.cpp \
    emoticons/emoticons.cpp \
    emoticons/emoticonselector.cpp \
    emoticons/emoticonsprovider.cpp \
    emoticons/emoticonstheme.cpp \
    emoticons/providers/adium/adium_emoticons.cpp \
    emoticons/providers/kde/kde_emoticons.cpp \
    emoticons/providers/pidgin/pidgin_emoticons.cpp \
    emoticons/providers/xmpp/xmpp_emoticons.cpp \
    idle/idle.cpp \
    main.cpp \
    mainchannel.cpp \
    network.cpp \
    networkreader.cpp \
    popup/popupmanager.cpp \
    popup/popupwindow.cpp \
    profilewidget.cpp \
    schatwindow.cpp \
    settings.cpp \
    settingsdialog.cpp \
    singleapplication.cpp \
    tab.cpp \
    trayicon.cpp \
    update/downloadmanager.cpp \
    update/update.cpp \
    update/updatewidget.cpp \
    updatexmlreader.cpp \
    welcomedialog.cpp \
    widget/inputwidget.cpp \
    widget/networkwidget.cpp \
    widget/sendwidget.cpp \
    widget/soundwidget.cpp \
    widget/userview.cpp \
    
contains( SCHAT_WEBKIT, 1 ) {
  HEADERS += \
    chatwindow/chatwindowstyle.h \
    chatwindow/chatwindowstyleoutput.h \
    
  SOURCES += \
    chatwindow/chatwindowstyle.cpp \
    chatwindow/chatwindowstyleoutput.cpp \
    
  RESOURCES += ../../data/$${TARGET}-webkit.qrc    
}
else {
  DEFINES += SCHAT_NO_WEBKIT
}

win32 {
    HEADERS += update/verifythread.h
    SOURCES += update/verifythread.cpp idle/idle_win.cpp
}
else {
    DEFINES += SCHAT_NO_UPDATE
}

unix {
#   LIBS += -lXss
#   DEFINES += SCHAT_X11_XSS
   SOURCES += idle/idle_x11.cpp
}
macx:SOURCES += idle/idle_mac.cpp

contains( SCHAT_STATIC, 1 ) {
    QTPLUGIN += qgif
    DEFINES  += SCHAT_STATIC
}

contains( SCHAT_BENCHMARK, 1 ) {
    DEFINES += SCHAT_BENCHMARK
}

TRANSLATIONS += ../../data/translations/schat_ru.ts
CODECFORTR = UTF-8

include(../common/common.pri)

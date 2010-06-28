# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_RESOURCES = 1
SCHAT_RC_FILE   = 1

QT += network
TEMPLATE = app

DEFINES += SCHAT_CLIENT

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
    colorbutton.h \
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
    privatetab.h \
    profilewidget.h \
    protocol.h \
    schatwindow.h \
    schatwindow_p.h \
    settings.h \
    settingsdialog.h \
    simplechatapp.h \
    soundaction.h \
    trayicon.h \
    update/downloadmanager.h \
    update/update.h \
    update/updatewidget.h \
    updatexmlreader.h \
    version.h \
    welcomedialog.h \
    widget/inputwidget.h \
    widget/networkwidget.h \
    widget/nickedit.h \
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
    colorbutton.cpp \
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
    privatetab.cpp \
    profilewidget.cpp \
    schatwindow.cpp \
    settings.cpp \
    settingsdialog.cpp \
    simplechatapp.cpp \
    soundaction.cpp \
    trayicon.cpp \
    update/downloadmanager.cpp \
    update/update.cpp \
    update/updatewidget.cpp \
    updatexmlreader.cpp \
    welcomedialog.cpp \
    widget/inputwidget.cpp \
    widget/networkwidget.cpp \
    widget/nickedit.cpp \
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
  QT += webkit   
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

contains( SCHAT_WINCE_VGA, 1 ) {
  SCHAT_RC_FILE = 0
  DEFINES += SCHAT_WINCE_VGA
  win32:RC_FILE = schat.rc
}

TRANSLATIONS += ../../data/translations/schat_ru.ts
CODECFORTR = UTF-8

unix {
  target.path += $$SCHAT_PREFIX/bin

  doc.files = ../../data/doc/ChangeLog.html 
  doc.path = $$SCHAT_PREFIX/share/schat/doc

  emoticons_kolobok.files = ../../data/emoticons/Kolobok/icondef.xml
  emoticons_kolobok.files += ../../data/emoticons/Kolobok/*.gif
  emoticons_kolobok.path = $$SCHAT_PREFIX/share/schat/emoticons/Kolobok

  emoticons_simple.files = $$quote(../../data/emoticons/Simple Smileys/emoticons.xml)
  emoticons_simple.files += $$quote(../../data/emoticons/Simple Smileys/*.png)
  emoticons_simple.path = $$quote($$SCHAT_PREFIX/share/schat/emoticons/Simple Smileys)

  networks.files = ../../data/networks/*.xml
  networks.path = $$SCHAT_PREFIX/share/schat/networks

  sounds.files = ../../data/sounds/*.wav
  sounds.path = $$SCHAT_PREFIX/share/schat/sounds

  INSTALLS += target doc emoticons_kolobok emoticons_simple networks sounds
}

include(../common/common.pri)

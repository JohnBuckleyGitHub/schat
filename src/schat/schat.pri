# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

macx {
   TARGET = SimpleChat
   SCHAT_RESOURCES = 0
   RESOURCES += ../../data/schat.qrc
   ICON = ../../os/macosx/schat.icns
   QMAKE_INFO_PLIST = ../../os/macosx/Info.plist
}
else {
   SCHAT_RESOURCES = 1
}

SCHAT_RC_FILE   = 1

QT += network
TEMPLATE = app

DEFINES += SCHAT_CLIENT

HEADERS += \
    3rdparty/qtwin.h \
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
    languagebox.h \
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
    text/PlainTextFilter.h \
    translatewidget.h \
    translation.h \
    trayicon.h \
    update/downloadmanager.h \
    update/update.h \
    update/updatewidget.h \
    updatexmlreader.h \
    version.h \
    widget/connectionstatus.h \
    widget/inputwidget.h \
    widget/networkwidget.h \
    widget/nickedit.h \
    widget/sendwidget.h \
    widget/soundwidget.h \
    widget/statusmenu.h \
    widget/userview.h \
    widget/userview_p.h \
    widget/welcome.h \

SOURCES += \
    3rdparty/qtwin.cpp \
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
    languagebox.cpp \
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
    text/PlainTextFilter.cpp \
    translatewidget.cpp \
    translation.cpp \
    trayicon.cpp \
    update/downloadmanager.cpp \
    update/update.cpp \
    update/updatewidget.cpp \
    updatexmlreader.cpp \
    widget/connectionstatus.cpp \
    widget/inputwidget.cpp \
    widget/networkwidget.cpp \
    widget/nickedit.cpp \
    widget/sendwidget.cpp \
    widget/soundwidget.cpp \
    widget/statusmenu.cpp \
    widget/userview.cpp \
    widget/welcome.cpp \
    
contains( SCHAT_WEBKIT, 1 ) {
  HEADERS += \
    chatwindow/chatwindowstyle.h \
    chatwindow/chatwindowstyleoutput.h \
    
  SOURCES += \
    chatwindow/chatwindowstyle.cpp \
    chatwindow/chatwindowstyleoutput.cpp \
    
  RESOURCES += ../../data/schat-webkit.qrc
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
#macx:SOURCES += idle/idle_mac.cpp

contains( SCHAT_STATIC, 1 ) {
    QTPLUGIN += qgif
    DEFINES  += SCHAT_STATIC
}

contains( SCHAT_WINCE_VGA, 1 ) {
  SCHAT_RC_FILE = 0
  DEFINES += SCHAT_WINCE_VGA
  win32:RC_FILE = schat.rc
}

TRANSLATIONS += ../../data/translations/schat_en.ts
TRANSLATIONS += ../../data/translations/schat_ru.ts
TRANSLATIONS += ../../data/translations/schat_uk.ts
CODECFORTR = UTF-8

unix {
  target.path += $$SCHAT_PREFIX/bin

  doc.files = ../../data/doc/ChangeLog.html

  emoticons_kolobok.files += ../../data/emoticons/Kolobok/icondef.xml
  emoticons_kolobok.files += ../../data/emoticons/Kolobok/*.gif

  emoticons_simple.files += $$quote(../../data/emoticons/Simple Smileys/emoticons.xml)
  emoticons_simple.files += $$quote(../../data/emoticons/Simple Smileys/*.png)

  networks.files = ../../data/networks/*.xml

  sounds.files = ../../data/sounds/*.wav

  translations.files = ../../data/translations/schat_en.qm
  translations.files += ../../data/translations/schat_ru.qm
  translations.files += ../../data/translations/schat_uk.qm
  translations.files += ../../data/translations/qt_ru.qm
  translations.files += ../../data/translations/qt_uk.qm
  translations.files += ../../data/translations/en.png
  translations.files += ../../data/translations/ru.png
  translations.files += ../../data/translations/uk.png

  macx {
    doc.path = ../../out/release/SimpleChat.app/Contents/Resources/doc
    emoticons_kolobok.path = ../../out/release/SimpleChat.app/Contents/Resources/emoticons/Kolobok
    emoticons_simple.path = $$quote(../../out/release/SimpleChat.app/Contents/Resources/emoticons/Simple Smileys)
    networks.path = ../../out/release/SimpleChat.app/Contents/Resources/networks
    sounds.path = ../../out/release/SimpleChat.app/Contents/Resources/sounds
    translations.path = ../../out/release/SimpleChat.app/Contents/Resources/translations

    INSTALLS += doc emoticons_kolobok emoticons_simple networks sounds translations
  } else {
    doc.path = $$SCHAT_PREFIX/share/doc/schat/html
    emoticons_kolobok.path = $$SCHAT_PREFIX/share/schat/emoticons/Kolobok
    emoticons_simple.path = $$quote($$SCHAT_PREFIX/share/schat/emoticons/Simple Smileys)
    networks.path = $$SCHAT_PREFIX/share/schat/networks
    sounds.path = $$SCHAT_PREFIX/share/schat/sounds
    translations.path = $$SCHAT_PREFIX/share/schat/translations

    applications.files = ../../os/ubuntu/schat.desktop
    applications.path = $$SCHAT_PREFIX/share/applications

    icon16.files = ../../data/images/icons/16x16/schat.png
    icon16.path = $$SCHAT_PREFIX/share/icons/hicolor/16x16/apps

    icon22.files = ../../data/images/icons/22x22/schat.png
    icon22.path = $$SCHAT_PREFIX/share/icons/hicolor/22x22/apps

    icon24.files = ../../res/images/icons/24x24/schat.png
    icon24.path = $$SCHAT_PREFIX/share/icons/hicolor/24x24/apps

    icon32.files = ../../data/images/icons/32x32/schat.png
    icon32.path = $$SCHAT_PREFIX/share/icons/hicolor/32x32/apps

    icon36.files = ../../data/images/icons/36x36/schat.png
    icon36.path = $$SCHAT_PREFIX/share/icons/hicolor/36x36/apps

    icon48.files = ../../data/images/icons/48x48/schat.png
    icon48.path = $$SCHAT_PREFIX/share/icons/hicolor/48x48/apps

    icon64.files = ../../data/images/icons/64x64/schat.png
    icon64.path = $$SCHAT_PREFIX/share/icons/hicolor/64x64/apps

    icon72.files = ../../data/images/icons/72x72/schat.png
    icon72.path = $$SCHAT_PREFIX/share/icons/hicolor/72x72/apps

    icon96.files = ../../data/images/icons/96x96/schat.png
    icon96.path = $$SCHAT_PREFIX/share/icons/hicolor/96x96/apps

    icon128.files = ../../data/images/icons/128x128/schat.png
    icon128.path = $$SCHAT_PREFIX/share/icons/hicolor/128x128/apps

    icon196.files = ../../data/images/icons/196x196/schat.png
    icon196.path = $$SCHAT_PREFIX/share/icons/hicolor/196x196/apps

    icon256.files = ../../data/images/icons/256x256/schat.png
    icon256.path = $$SCHAT_PREFIX/share/icons/hicolor/256x256/apps

    icon512.files = ../../data/images/icons/512x512/schat.png
    icon512.path = $$SCHAT_PREFIX/share/icons/hicolor/512x512/apps

    INSTALLS += target doc emoticons_kolobok emoticons_simple networks sounds translations applications icon16 icon22 icon24 icon32 icon36 icon48 icon64 icon72 icon96 icon128 icon196 icon256 icon512
  }
}

win32:LIBS += -luser32

include(migrate.pri)
include(../common/common.pri)

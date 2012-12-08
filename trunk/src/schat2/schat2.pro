# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2012 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_RESOURCES   = 0
SCHAT_RC_FILE     = 1
SCHAT_SINGLEAPP   = 0

macx {
   TARGET = SimpleChat2
   ICON = ../../os/macosx/schat.icns
   QMAKE_INFO_PLIST = ../../os/macosx/Info.plist
}

QT = core gui network webkit
TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

HEADERS = \
    ChatApp.h \
    debugstream.h \
    ui/ChatWindow.h \

SOURCES = \
    ChatApp.cpp \
    main.cpp \
    ui/ChatWindow.cpp \
    
win32:contains(QT_VERSION, ^4.*) {
    HEADERS += qtwin/qtwin.h
    SOURCES += qtwin/qtwin.cpp
}

DEFINES += SCHAT_WEBKIT

SCHAT_CLIENT_LIB = 1
SCHAT_CORE_LIB = 1

translations.files += ../../res/translations/schat2_ru.qm
translations.files += ../../res/translations/schat2_en.qm
translations.files += ../../res/translations/ru.png

sounds.files += ../../res/sounds/Received.wav

unix {  
  macx {
    translations.path += ../../out/SimpleChat2.app/Contents/Resources/translations
    sounds.path  += ../../out/SimpleChat2.app/Contents/Resources/sounds/

    INSTALLS += translations sounds
  } else {
    icon16.files = ../../res/images/icons/16x16/schat2.png
    icon16.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/16x16/apps

    icon22.files = ../../res/images/icons/22x22/schat2.png
    icon22.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/22x22/apps

    icon24.files = ../../res/images/icons/24x24/schat2.png
    icon24.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/24x24/apps

    icon32.files = ../../res/images/icons/32x32/schat2.png
    icon32.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/32x32/apps

    icon36.files = ../../res/images/icons/36x36/schat2.png
    icon36.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/36x36/apps

    icon48.files = ../../res/images/icons/48x48/schat2.png
    icon48.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/48x48/apps

    icon64.files = ../../res/images/icons/64x64/schat2.png
    icon64.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/64x64/apps

    icon72.files = ../../res/images/icons/72x72/schat2.png
    icon72.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/72x72/apps

    icon96.files = ../../res/images/icons/96x96/schat2.png
    icon96.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/96x96/apps

    icon128.files = ../../res/images/icons/128x128/schat2.png
    icon128.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/128x128/apps

    icon196.files = ../../res/images/icons/196x196/schat2.png
    icon196.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/196x196/apps

    icon256.files = ../../res/images/icons/256x256/schat2.png
    icon256.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/256x256/apps

    icon512.files = ../../res/images/icons/512x512/schat2.png
    icon512.path = $$SCHAT_PREFIX/usr/share/icons/hicolor/512x512/apps

    applications.files = ../../os/ubuntu/schat2.desktop
    applications.path = $$SCHAT_PREFIX/usr/share/applications

    translations.path = $$SCHAT_PREFIX/usr/share/schat2/translations
    sounds.path  += $$SCHAT_PREFIX/usr/share/schat2/sounds/
    target.path += $$SCHAT_PREFIX/usr/bin
    
    INSTALLS += target applications translations sounds icon16 icon22 icon24 icon32 icon36 icon48 icon64 icon72 icon96 icon128 icon196 icon256 icon512
  }
}

win32 {
  translations.files += $$[QT_INSTALL_PREFIX]/translations/qt_ru.qm
  translations.path += ../../os/win32/schat2/translations

  qt.files += $$[QT_INSTALL_PREFIX]/bin/libeay32.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/ssleay32.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/libpng15.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/zlib.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/msvcp100.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/msvcr100.dll 
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtCore4.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtGui4.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtNetwork4.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtSql4.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtWebKit4.dll
  qt.path += ../../os/win32/schat2/

  imageformats.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qgif4.dll
  imageformats.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qico4.dll
  imageformats.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qjpeg4.dll
  imageformats.path += ../../os/win32/schat2/plugins/qt/imageformats
  
  sqldrivers.files += $$[QT_INSTALL_PREFIX]/plugins/sqldrivers/qsqlite4.dll
  sqldrivers.path += ../../os/win32/schat2/plugins/qt/sqldrivers

  doc.files += ../../res/doc/ChangeLog.html
  doc.path += ../../os/win32/schat2/doc/

  sounds.path += ../../os/win32/schat2/sounds/

  target.path += ../../os/win32/schat2/
  INSTALLS += target translations qt imageformats sqldrivers doc sounds
}

include(../common/config.pri)
include(../common/common.pri)

contains( SCHAT_SINGLE_APP, 1 ) {
    HEADERS += qtsingleapplication/qtsingleapplication.h qtsingleapplication/qtlocalpeer.h
    SOURCES += qtsingleapplication/qtsingleapplication.cpp qtsingleapplication/qtlocalpeer.cpp
}
else {
    DEFINES += SCHAT_NO_SINGLEAPP
}

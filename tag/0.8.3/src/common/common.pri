# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2011 IMPOMEZIA <schat@impomezia.com>
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

DEPENDPATH += \
    . \
    ../common \

INCLUDEPATH += \
    . \
    ../common \

CONFIG(debug, debug|release) { 
  RCC_DIR = ../../tmp/$${TARGET}/debug/rcc
  MOC_DIR = ../../tmp/$${TARGET}/debug/moc
  OBJECTS_DIR = ../../tmp/$${TARGET}/debug/obj
  DESTDIR = ../../out/debug
}

CONFIG(release, debug|release) { 
  RCC_DIR = ../../tmp/$${TARGET}/release/rcc
  MOC_DIR = ../../tmp/$${TARGET}/release/moc
  OBJECTS_DIR = ../../tmp/$${TARGET}/release/obj
  DESTDIR = ../../out/release
}

contains( SCHAT_WINCE, 1 ) {
  SCHAT_SINGLEAPP = 0
}

macx {
  DEFINES += SCHAT_NO_STYLE
}
else {
  DEFINES += SCHAT_NO_MENU
}

contains( SCHAT_SINGLEAPP, 0 ) {
  DEFINES += SCHAT_NO_SINGLEAPP
}
else {
  include(../3rdparty/qtsingleapplication/src/qtsingleapplication.pri)
}

contains( SCHAT_CONSOLE, 1 ) {
  CONFIG += console
}
else {
  CONFIG -= console
}

contains( SCHAT_DEBUG, 1 ) {
  DEFINES += SCHAT_DEBUG
}

contains( SCHAT_RESOURCES, 1 ) {
  RESOURCES += ../../data/$${TARGET}.qrc
}

contains( SCHAT_RC_FILE, 1 ) {
  win32:RC_FILE = $${TARGET}.rc
}

contains( SCHAT_DEVEL_MODE, 1 ) {
  DEFINES += SCHAT_DEVEL_MODE
}


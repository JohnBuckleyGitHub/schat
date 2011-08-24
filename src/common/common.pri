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
    ../3rdparty \

INCLUDEPATH += \
    . \
    ../common \
    ../3rdparty \

!win32:VERSION = 1.9.2

CONFIG(debug, debug|release) { 
  RCC_DIR = ../../tmp/$${TEMPLATE}/$${TARGET}/debug/rcc
  MOC_DIR = ../../tmp/$${TEMPLATE}/$${TARGET}/debug/moc
  OBJECTS_DIR = ../../tmp/$${TEMPLATE}/$${TARGET}/debug/obj
  DESTDIR = ../../out/debug
} else { 
  RCC_DIR = ../../tmp/$${TEMPLATE}/$${TARGET}/release/rcc
  MOC_DIR = ../../tmp/$${TEMPLATE}/$${TARGET}/release/moc
  OBJECTS_DIR = ../../tmp/$${TEMPLATE}/$${TARGET}/release/obj
  DESTDIR = ../../out
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

contains( SCHAT_USE_SSL, 0 ) {
  DEFINES += SCHAT_NO_SSL
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

contains( SCHAT_CLIENT_LIB, 1 ) {
  CONFIG(debug, debug|release) {
    LIBS += -L../../out/debug -lschat-client
  } else {
    LIBS += -L../../out -lschat-client
  }
}

contains( SCHAT_CORE_LIB, 1 ) {
  CONFIG(debug, debug|release) {
    LIBS += -L../../out/debug -lschat
  } else {
    LIBS += -L../../out -lschat
  }
}

contains( SCHAT_DAEMON_LIB, 1 ) {
  CONFIG(debug, debug|release) {
    LIBS += -L../../out/debug -lschatd
  } else {
    LIBS += -L../../out -lschatd
  }
}


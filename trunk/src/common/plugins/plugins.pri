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

TEMPLATE = lib
CONFIG   += plugin

DEPENDPATH += \
    . \
    ../ \
    ../../ \
    ../../../3rdparty \

INCLUDEPATH += \
    . \
    ../ \
    ../../ \
    ../../../3rdparty \

CONFIG(debug, debug|release) { 
  RCC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/debug/rcc
  MOC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/debug/moc
  OBJECTS_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/debug/obj
  DESTDIR = ../../../../out/debug/plugins
} else { 
  RCC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/release/rcc
  MOC_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/release/moc
  OBJECTS_DIR = ../../../../tmp/$${TEMPLATE}/$${TARGET}/release/obj
  DESTDIR = ../../../../out/plugins
}

contains( SCHAT_CLIENT_LIB, 1 ) {
  CONFIG(debug, debug|release) { 
    LIBS += -L../../../../out/debug -lschat-client 
  } else { 
    LIBS += -L../../../../out -lschat-client  
  }
}

contains( SCHAT_CORE_LIB, 1 ) {
  DEPENDPATH += \
    ../../../schat2 \

  INCLUDEPATH += \
    ../../../schat2 \
    
  CONFIG(debug, debug|release) {
    LIBS += -L../../../../out/debug -lschat
  } else {
    LIBS += -L../../../../out -lschat
  }
}

contains( SCHAT_DAEMON_LIB, 1 ) {
  DEPENDPATH += \
    ../../../schatd2 \

  INCLUDEPATH += \
    ../../../schatd2 \
    
  CONFIG(debug, debug|release) {
    LIBS += -L../../../../out/debug -lschatd
  } else {
    LIBS += -L../../../../out -lschatd
  }
}

win32:RC_FILE = $${TARGET}.rc


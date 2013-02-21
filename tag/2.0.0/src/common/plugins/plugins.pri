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

TEMPLATE      = lib
CONFIG        += plugin
PLUGIN_TARGET = schat2

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

contains( SCHAT_DEBUG, 1 ) {
  DEFINES += SCHAT_DEBUG
}

contains( SCHAT_CLIENT_LIB, 1 ) {
  CONFIG(debug, debug|release) { 
    LIBS = -L../../../../out/debug -lschat-client $${LIBS}
  } else { 
    LIBS = -L../../../../out -lschat-client $${LIBS}
  }
}

contains( SCHAT_CORE_LIB, 1 ) {
  DEPENDPATH += ../../../schat2
  INCLUDEPATH += ../../../schat2

  CONFIG(debug, debug|release) {
    LIBS = -L../../../../out/debug -lschat $${LIBS}
  } else {
    LIBS = -L../../../../out -lschat $${LIBS}
  }
}

contains( SCHAT_DAEMON_LIB, 1 ) {
  DEPENDPATH += ../../../schatd2
  INCLUDEPATH += ../../../schatd2

  CONFIG(debug, debug|release) {
    LIBS = -L../../../../out/debug -lschatd $${LIBS}
  } else {
    LIBS = -L../../../../out -lschatd $${LIBS}
  }

  PLUGIN_TARGET = schatd2
}

contains( SCHAT_REST_LIB, 1 ) {
  DEPENDPATH += ../../../rest
  INCLUDEPATH += ../../../rest

  CONFIG(debug, debug|release) {
    LIBS = -L../../../../out/debug -lschat-rest $${LIBS}
  } else {
    LIBS = -L../../../../out -lschat-rest $${LIBS}
  }
}

contains( SCHAT_TUFAO_LIB, 1 ) {
  CONFIG(debug, debug|release) {
    LIBS = -L../../../../out/debug -lschat-tufao $${LIBS}
  } else {
    LIBS = -L../../../../out -lschat-tufao $${LIBS}
  }
}

win32:RC_FILE = $${TARGET}.rc

macx:contains( PLUGIN_TARGET, schat2 ) {
  macx:target.path += ../../../../out/SimpleChat2.app/Contents/PlugIns/
  INSTALLS += target
}

unix:!macx {
  target.path += $$SCHAT_PREFIX/usr/share/$${PLUGIN_TARGET}/plugins
  INSTALLS += target
}

win32 {
  doc.files += res/doc/*.html
  doc.path += ../../../../os/win32/$${PLUGIN_TARGET}/doc

  target.path += ../../../../os/win32/$${PLUGIN_TARGET}/plugins
  INSTALLS += target doc
}

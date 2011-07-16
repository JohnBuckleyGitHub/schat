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
    ../ \

INCLUDEPATH += \
    . \
    ../ \

CONFIG(debug, debug|release) { 
  RCC_DIR = ../../../tmp/$${TARGET}/debug/rcc
  MOC_DIR = ../../../tmp/$${TARGET}/debug/moc
  OBJECTS_DIR = ../../../tmp/$${TARGET}/debug/obj
  DESTDIR = ../../../out/debug/plugins/$${PLUGIN_GROUP}
}

CONFIG(release, debug|release) { 
  RCC_DIR = ../../../tmp/$${TARGET}/release/rcc
  MOC_DIR = ../../../tmp/$${TARGET}/release/moc
  OBJECTS_DIR = ../../../tmp/$${TARGET}/release/obj
  DESTDIR = ../../../out/release/plugins/$${PLUGIN_GROUP}
}


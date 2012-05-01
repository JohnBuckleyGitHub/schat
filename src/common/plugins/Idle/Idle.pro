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

SCHAT_CLIENT_LIB = 1
SCHAT_CORE_LIB = 1
QT = core gui

HEADERS  = \
   Idle.h \
   IdlePlugin.h \
   IdlePlugin_p.h \

SOURCES  = \
   Idle.cpp \
   IdlePlugin.cpp \
   
win32:SOURCES += idle_win.cpp

unix {
  macx:target.path += ../../../../out/SimpleChat2.app/Contents/PlugIns/
  !macx:target.path += $$SCHAT_PREFIX/usr/share/schat2/plugins
  INSTALLS += target
}

include(../plugins.pri)

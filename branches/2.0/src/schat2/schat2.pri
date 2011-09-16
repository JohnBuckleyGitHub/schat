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

SCHAT_RESOURCES   = 0
SCHAT_RC_FILE     = 1
SCHAT_SINGLEAPP   = 0

QT = core gui network webkit
TEMPLATE = app

HEADERS = \
    ChatApp.h \
    debugstream.h \
    ui/ChatWindow.h \

SOURCES = \
    ChatApp.cpp \
    main.cpp \
    ui/ChatWindow.cpp \
    
win32 {
    HEADERS += qtwin/qtwin.h
    SOURCES += qtwin/qtwin.cpp
}

DEFINES += SCHAT_WEBKIT

SCHAT_CLIENT_LIB = 1
SCHAT_CORE_LIB = 1

unix {
  pixmaps.files = ../../data/images/schat2.png
  pixmaps.path = $$SCHAT_PREFIX/usr/share/pixmaps

  applications.files = ../../os/ubuntu/schat2.desktop
  applications.path = $$SCHAT_PREFIX/usr/share/applications
  
  target.path += $$SCHAT_PREFIX/usr/bin
  INSTALLS += target pixmaps applications
}

include(../common/common.pri)


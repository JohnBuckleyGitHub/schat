# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_CONSOLE    = 1
SCHAT_DEBUG      = 0
SCHAT_RESOURCES  = 1
SCHAT_RC_FILE    = 1
SCHAT_LOCAL_IPC  = 0

CONFIG += exceptions

QT = core network
TEMPLATE = app

DEFINES += SCHAT_NO_WRITE_SETTINGS
DEFINES += _WIN32_WINNT=0x0501

win32 {
    LIBS += -lws2_32 -lmswsock
}

HEADERS = \
    abstractprofile.h \
    chatdaemon.h \
    chatserver.h \
    connection.h \
    version.h \
    workerpool.h \

SOURCES = \
    abstractprofile.cpp \
    chatdaemon.cpp \
    chatserver.cpp \
    connection.cpp \
    main.cpp \
    workerpool.cpp \

contains( SCHAT_LOCAL_IPC, 1 ) {
  HEADERS += ipc/localservice.h
  SOURCES += ipc/localservice.cpp
}
else {
  DEFINES += SCHAT_NO_LOCAL_SERVER
}

TRANSLATIONS += ../../data/translations/schatd_ru.ts
CODECFORTR = UTF-8

include(../common/common.pri)

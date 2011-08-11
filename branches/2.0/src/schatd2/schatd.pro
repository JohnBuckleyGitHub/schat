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

SCHAT_RESOURCES = 0
SCHAT_DEBUG     = 1
SCHAT_USE_SSL   = 1

TEMPLATE = lib
QT = core network sql
TARGET = schatd
DEFINES += SCHAT_LIBRARY
win32:RC_FILE = schatd.rc

HEADERS = \
    cores/Core.h \
    cores/GenericCore.h \
    events.h \
    DataBase.h \
    ServerChannel.h \
    ServerUser.h \
    Storage.h \

SOURCES = \
    cores/Core.cpp \
    cores/GenericCore.cpp \
    events.cpp \
    DataBase.cpp \
    ServerChannel.cpp \
    ServerUser.cpp \
    Storage.cpp \

include(../common/client.pri)

TRANSLATIONS += ../../data/translations/schatd-lib_en.ts
TRANSLATIONS += ../../data/translations/schatd-lib_ru.ts
CODECFORTR = UTF-8

include(../common/common.pri)

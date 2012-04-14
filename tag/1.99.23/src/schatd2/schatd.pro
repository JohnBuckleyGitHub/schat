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

SCHAT_RESOURCES = 0
SCHAT_DEBUG     = 1
SCHAT_USE_SSL   = 1

TEMPLATE = lib
QT = core network sql
TARGET = schatd
DEFINES += SCHAT_LIBRARY
win32:RC_FILE = schatd.rc
DEFINES += SCHAT_DAEMON
SCHAT_LITTLE_CLIENT = 1

HEADERS = \
    Ch.h \
    cores/AnonymousAuth.h \
    cores/CookieAuth.h \
    cores/Core.h \
    cores/NodeAuth.h \
    cores/PasswordAuth.h \
    DataBase.h \
    events.h \
    feeds/NodeAclFeed.h \
    feeds/NodeTopicFeed.h \
    Hosts.h \
    net/NodeAuthReply.h \
    net/NodePool.h \
    net/NodeWorker.h \
    net/TcpServer.h \
    NodeLog.h \
    NodeNoticeReader.h \
    NodePlugins.h \
    Normalize.h \
    plugins/GeoHook.h \
    plugins/NodePlugin.h \
    ServerChannel.h \
    Sockets.h \
    Storage.h \

SOURCES = \
    Ch.cpp \
    cores/AnonymousAuth.cpp \
    cores/CookieAuth.cpp \
    cores/Core.cpp \
    cores/PasswordAuth.cpp \
    DataBase.cpp \
    events.cpp \
    feeds/NodeAclFeed.cpp \
    feeds/NodeTopicFeed.cpp \
    Hosts.cpp \
    net/NodeAuthReply.cpp \
    net/NodePool.cpp \
    net/NodeWorker.cpp \
    net/TcpServer.cpp \
    NodeLog.cpp \
    NodeNoticeReader.cpp \
    NodePlugins.cpp \
    Normalize.cpp \
    plugins/GeoHook.cpp \
    plugins/NodePlugin.cpp \
    ServerChannel.cpp \
    Sockets.cpp \
    Storage.cpp \

include(../common/client.pri)

TRANSLATIONS += ../../data/translations/schatd-lib_en.ts
TRANSLATIONS += ../../data/translations/schatd-lib_ru.ts
CODECFORTR = UTF-8

unix:!macx {
  target.path += $$SCHAT_PREFIX/usr/lib
  INSTALLS += target
}

include(../common/common.pri)

# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_RESOURCES = 1

TEMPLATE = lib
QT = core network sql
TARGET = schatd
DEFINES += SCHAT_LIBRARY
win32:RC_FILE = schatd.rc
DEFINES += SCHAT_DAEMON
SCHAT_LITTLE_CLIENT = 1

HEADERS = \
    Ch.h \
    ChHook.h \
    cores/AnonymousAuth.h \
    cores/CookieAuth.h \
    cores/Core.h \
    cores/DiscoveryAuth.h \
    cores/ExternalAuth.h \
    cores/NodeAuth.h \
    DataBase.h \
    events.h \
    feeds/FeedsCore.h \
    feeds/NodeAclFeed.h \
    Host.h \
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
    SettingsHook.h \
    Sockets.h \
    Storage.h \
    StorageHook.h \
    tools/CmdLine.h \
    User.h \

SOURCES = \
    Ch.cpp \
    ChHook.cpp \
    cores/AnonymousAuth.cpp \
    cores/CookieAuth.cpp \
    cores/Core.cpp \
    cores/DiscoveryAuth.cpp \
    cores/ExternalAuth.cpp \
    DataBase.cpp \
    events.cpp \
    feeds/FeedsCore.cpp \
    feeds/NodeAclFeed.cpp \
    Host.cpp \
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
    SettingsHook.cpp \
    Sockets.cpp \
    Storage.cpp \
    StorageHook.cpp \
    tools/CmdLine.cpp \
    User.cpp \

include(../common/client.pri)

unix:!macx {
  include(../common/prefix.pri)

  target.path += $${LIBDIR}
  INSTALLS += target
}

win32 {
  target.path += ../../os/win32/schatd2/
  INSTALLS += target
}

include(../common/config.pri)
include(../common/common.pri)

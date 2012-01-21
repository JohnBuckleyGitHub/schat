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

HEADERS += \
    Account.h \
    acl/Acl.h \
    acl/Groups.h \
    base32/base32.h \
    Channel.h \
    client/AbstractClient.h \
    client/AbstractClient_p.h \
    client/NetworkPool.h \
    DateTime.h \
    feeds/Feed.h \
    feeds/FeedHeader.h \
    feeds/Feeds.h \
    feeds/FeedStorage.h \
    FileLocations.h \
    Gender.h \
    JSON.h \
    k8json/k8json.h \
    net/PacketReader.h \
    net/packets/auth.h \
    net/packets/ChannelNotice.h \
    net/packets/FeedNotice.h \
    net/packets/MessageNotice.h \
    net/packets/Notice.h \
    net/PacketWriter.h \
    net/SimpleID.h \
    net/SimpleSocket.h \
    net/SimpleSocket_p.h \
    net/TransportReader.h \
    net/TransportWriter.h \
    Plugins.h \
    plugins/Ver.h \
    schat.h \
    Settings.h \
    sglobal.h \
    text/HtmlFilter.h \
    text/HtmlToken.h \
    text/PlainTextFilter.h \
    Translation.h \

SOURCES += \
    Account.cpp \
    acl/Acl.cpp \
    base32/base32.cpp \
    Channel.cpp \
    client/AbstractClient.cpp \
    client/NetworkPool.cpp \
    DateTime.cpp \
    feeds/Feed.cpp \
    feeds/FeedHeader.cpp \
    feeds/Feeds.cpp \
    feeds/FeedStorage.cpp \
    FileLocations.cpp \
    Gender.cpp \
    JSON.cpp \
    k8json/k8json.cpp \
    net/PacketReader.cpp \
    net/packets/auth.cpp \
    net/packets/ChannelNotice.cpp \
    net/packets/FeedNotice.cpp \
    net/packets/MessageNotice.cpp \
    net/packets/Notice.cpp \
    net/SimpleID.cpp \
    net/SimpleSocket.cpp \
    net/TransportReader.cpp \
    net/TransportWriter.cpp \
    Plugins.cpp \
    plugins/Ver.cpp \
    Settings.cpp \
    text/HtmlFilter.cpp \
    text/PlainTextFilter.cpp \
    Translation.cpp \

!contains( SCHAT_LITTLE_CLIENT, 1 ) {
  HEADERS += \
    client/ChatClient.h \
    client/ClientChannels.h \
    client/ClientCmd.h \
    client/ClientFeeds.h \
    client/ClientHooks.h \
    client/ClientMessages.h \
    client/SimpleClient.h \
    client/SimpleClient_p.h \
    
  SOURCES += \
    client/ChatClient.cpp \
    client/ClientChannels.cpp \
    client/ClientCmd.cpp \
    client/ClientFeeds.cpp \
    client/ClientHooks.cpp \
    client/ClientMessages.cpp \
    client/SimpleClient.cpp \
}

include(../3rdparty/jdns/jdns.pri)

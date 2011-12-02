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
    feeds/AccountFeed.h \
    feeds/Feed.h \
    feeds/FeedFactory.h \
    feeds/FeedHeader.h \
    feeds/Feeds.h \
    feeds/TopicFeed.h \
    FileLocations.h \
    Gender.h \
    k8json/k8json.h \
    net/PacketReader.h \
    net/packets/AbstractPacket.h \
    net/packets/accounts.h \
    net/packets/auth.h \
    net/packets/ChannelPacket.h \
    net/packets/message.h \
    net/packets/Notice.h \
    net/packets/users.h \
    net/PacketWriter.h \
    net/ServerData.h \
    net/SimpleID.h \
    net/SimpleSocket.h \
    net/SimpleSocket_p.h \
    net/TransportReader.h \
    net/TransportWriter.h \
    Plugins.h \
    schat.h \
    Settings.h \
    SimpleJSon.h \
    text/HtmlFilter.h \
    text/HtmlToken.h \
    text/PlainTextFilter.h \
    Translation.h \
    User.h \

SOURCES += \
    Account.cpp \
    acl/Acl.cpp \
    base32/base32.cpp \
    Channel.cpp \
    client/AbstractClient.cpp \
    client/NetworkPool.cpp \
    DateTime.cpp \
    feeds/AccountFeed.cpp \
    feeds/Feed.cpp \
    feeds/FeedFactory.cpp \
    feeds/FeedHeader.cpp \
    feeds/Feeds.cpp \
    feeds/TopicFeed.cpp \
    FileLocations.cpp \
    Gender.cpp \
    k8json/k8json.cpp \
    net/PacketReader.cpp \
    net/packets/accounts.cpp  \
    net/packets/auth.cpp \
    net/packets/ChannelPacket.cpp \
    net/packets/message.cpp \
    net/packets/Notice.cpp \
    net/packets/users.cpp \
    net/ServerData.cpp \
    net/SimpleID.cpp \
    net/SimpleSocket.cpp \
    net/TransportReader.cpp \
    net/TransportWriter.cpp \
    Plugins.cpp \
    Settings.cpp \
    SimpleJSon.cpp \
    text/HtmlFilter.cpp \
    text/PlainTextFilter.cpp \
    Translation.cpp \
    User.cpp \

!contains( SCHAT_LITTLE_CLIENT, 1 ) {
  HEADERS += \
    client/ChatClient.h \
    client/ClientChannels.h \
    client/ClientCmd.h \
    client/ClientHelper.h \
    client/SimpleClient.h \
    client/SimpleClient_p.h \
    
  SOURCES += \
    client/ChatClient.cpp \
    client/ClientChannels.cpp \
    client/ClientCmd.cpp \
    client/ClientHelper.cpp \
    client/SimpleClient.cpp \
}

include(../3rdparty/jdns/jdns.pri)

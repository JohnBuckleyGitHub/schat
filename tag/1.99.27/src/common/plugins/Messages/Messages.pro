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

SCHAT_DAEMON_LIB = 1
QT = core network sql

HEADERS  = \
   feeds/NodeHistoryFeed.h \
   MessagesCh.h \
   MessagesPlugin.h \
   MessagesPlugin_p.h \
   NodeMessages.h \
   NodeMessagesDB.h \

SOURCES  = \
   feeds/NodeHistoryFeed.cpp \
   MessagesCh.cpp \
   MessagesPlugin.cpp \
   NodeMessages.cpp \
   NodeMessagesDB.cpp \

unix:!macx {
  target.path += $$SCHAT_PREFIX/usr/share/schatd2/plugins
  INSTALLS += target
}

include(../plugins.pri)

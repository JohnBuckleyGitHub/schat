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
OTHER_FILES += Channels.json

HEADERS  = \
   ChannelsCmd.h \
   ChannelsMenuImpl.h \
   ChannelsPlugin.h \
   ChannelsPlugin_p.h \
   ChannelsSettings.h \

SOURCES  = \
   ChannelsCmd.cpp \
   ChannelsMenuImpl.cpp \
   ChannelsPlugin.cpp \
   ChannelsSettings.cpp \

RESOURCES += Channels.qrc

TRANSLATIONS += res/translations/channels_en.ts
TRANSLATIONS += res/translations/channels_ru.ts
CODECFORTR = UTF-8

include(../plugins.pri)

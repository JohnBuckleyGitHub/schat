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
QT = core network

DEFINES += GEOIPDATADIR=\\\"./res\\\"

win32 {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    LIBS += -lwsock32 -lws2_32
}

HEADERS  = \
   GeoIP/GeoIP.h \
   GeoIP/GeoIP_internal.h \
   GeoIP/GeoIPCity.h \
   GeoIP/global.h \
   GeoIP/types.h \
   GeoIPImpl.h \
   GeoIPPlugin.h \
   GeoIPPlugin_p.h \
   QGeoIP.h \

SOURCES  = \
   GeoIP/GeoIP.c \
   GeoIP/GeoIPCity.c \
   GeoIP/regionName.c \
   GeoIP/timeZone.c \
   GeoIPImpl.cpp \
   GeoIPPlugin.cpp \
   QGeoIP.cpp \

include(../plugins.pri)

# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008 IMPOMEZIA <schat@impomezia.com>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

SCHAT_CONSOLE    = 0
SCHAT_DEBUG      = 0
SCHAT_SINGLE_APP = 0
SCHAT_RESOURCES  = 0
SCHAT_RC_FILE    = 1
SCHAT_LOCAL_IPC  = 1

QT = core network
TEMPLATE = app

DEFINES += SCHAT_NO_WRITE_SETTINGS

HEADERS = \
    abstractprofile.h \
    abstractsettings.h \
    channellog.h \
    clientservice.h \
    daemon.h \
    daemonservice.h \
    daemonsettings.h \
    linkunit.h \
    log.h \
    network.h \
    networkreader.h \
    protocol.h \
    userunit.h \
    version.h \

SOURCES = \
    abstractprofile.cpp \
    abstractsettings.cpp \
    channellog.cpp \
    clientservice.cpp \
    daemon.cpp \
    daemonservice.cpp \
    daemonsettings.cpp \
    linkunit.cpp \
    log.cpp \
    main.cpp \
    network.cpp \
    networkreader.cpp \
    userunit.cpp \

contains( SCHAT_LOCAL_IPC, 1 ) {
  HEADERS += ipc/localservice.h
  SOURCES += ipc/localservice.cpp
}
else {
  DEFINES += SCHAT_NO_LOCAL_SERVER
}

include(../common/common.pri)

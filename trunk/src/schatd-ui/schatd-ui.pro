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

SCHAT_CONSOLE    = 0
SCHAT_DEBUG      = 0
SCHAT_RESOURCES  = 1
SCHAT_RC_FILE    = 1
SCHAT_DEVEL_MODE = 0

QT += network
TEMPLATE = app

HEADERS += \
    abstractsettings.h \
    abstractsettingsdialog.h \
    daemonsettings.h \
    daemonsettingsdialog.h \
    daemonui.h \
    ipc/localclientservice.h \
    network.h \
    networkreader.h \
    networkwriter.h \
    singleapplication.h \

SOURCES += \
    abstractsettings.cpp \
    abstractsettingsdialog.cpp \
    daemonsettings.cpp \
    daemonsettingsdialog.cpp \
    daemonui.cpp \
    ipc/localclientservice.cpp \
    main.cpp \
    network.cpp \
    networkreader.cpp \
    networkwriter.cpp \
    singleapplication.cpp \

win32{
    HEADERS += serviceinstaller.h
    SOURCES += serviceinstaller.cpp
}
else {
    DEFINES += SCHATD_NO_SERVICE
}

include(../common/common.pri)

# $Id$
# IMPOMEZIA Simple Chat
# Copyright (c) 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

SCHAT_CONSOLE    = 1
SCHAT_DEBUG      = 0
SCHAT_RESOURCES  = 0
SCHAT_RC_FILE    = 0
SCHAT_SINGLEAPP  = 0
BENCHMARK_NO_UI  = 1

QT = core network
TEMPLATE = app

DEFINES += SCHAT_NO_WRITE_SETTINGS
DEFINES += SCHAT_CLIENT

HEADERS = \
    abstractprofile.h \
    abstractsettings.h \
    benchmark.h \
    benchmarkapp.h \
    clientservice.h \
    network.h \
    networkreader.h \

SOURCES = \
    abstractprofile.cpp \
    abstractsettings.cpp \
    benchmark.cpp \
    benchmarkapp.cpp \
    clientservice.cpp \
    main.cpp \
    network.cpp \
    networkreader.cpp \

CODECFORTR = UTF-8

contains( BENCHMARK_NO_UI, 1 ) {
  CONFIG += console
  DEFINES += BENCHMARK_NO_UI
}
else {
  CONFIG += console
  QT += gui
  HEADERS += benchmarkui.h
  SOURCES += benchmarkui.cpp
}

include(../common/common.pri)

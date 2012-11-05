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
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

QT = core network
TEMPLATE = app

DEFINES += SCHAT_NO_WRITE_SETTINGS

HEADERS = \
    abstractprofile.h \
    abstractsettings.h \
    channellog.h \
    clientservice.h \
    daemon.h \
    daemonapp.h \
    daemonlog.h \
    daemonservice.h \
    daemonsettings.h \
    linkunit.h \
    network.h \
    networkreader.h \
    normalizereader.h \
    protocol.h \
    text/PlainTextFilter.h \
    translation.h \
    userunit.h \
    version.h \

SOURCES = \
    abstractprofile.cpp \
    abstractsettings.cpp \
    channellog.cpp \
    clientservice.cpp \
    daemon.cpp \
    daemonapp.cpp \
    daemonlog.cpp \
    daemonservice.cpp \
    daemonsettings.cpp \
    linkunit.cpp \
    main.cpp \
    network.cpp \
    networkreader.cpp \
    normalizereader.cpp \
    text/PlainTextFilter.cpp \
    translation.cpp \
    userunit.cpp \

contains( SCHAT_LOCAL_IPC, 1 ) {
  HEADERS += ipc/localservice.h
  SOURCES += ipc/localservice.cpp
}
else {
  DEFINES += SCHAT_NO_LOCAL_SERVER
}

TRANSLATIONS += ../../data/translations/schatd_en.ts
TRANSLATIONS += ../../data/translations/schatd_ru.ts
TRANSLATIONS += ../../data/translations/schatd_uk.ts
CODECFORTR = UTF-8

unix:!macx {
  etc.files = ../../data/normalize.xml
  etc.files += ../../data/motd.html
  etc.files += ../../data/schatd.conf.example

  translations.files = ../../data/translations/schatd_en.qm
  translations.files += ../../data/translations/schatd_ru.qm
  translations.files += ../../data/translations/schatd_uk.qm
  translations.files += ../../data/translations/qt_ru.qm
  translations.files += ../../data/translations/qt_uk.qm

  userbar.files = ../../misc/tools/userbar/userbar.php
  userbar.files += ../../misc/tools/userbar/userbar-template.png
  userbar.files += ../../misc/tools/userbar/hoog_mini.ttf

  target.path += $$SCHAT_PREFIX/usr/sbin
  etc.path = $$SCHAT_PREFIX/etc/schatd
  translations.path = $$SCHAT_PREFIX/usr/share/schatd/translations
  userbar.path = $$SCHAT_PREFIX/usr/share/schatd/userbar

  INSTALLS += target etc translations userbar
}

include(../3rdparty/qtservice/src/qtservice.pri)
include(../common/common.pri)


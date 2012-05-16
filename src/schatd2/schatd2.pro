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

SCHAT_RESOURCES   = 0
SCHAT_RC_FILE     = 1
SCHAT_SINGLEAPP   = 0

QT = core network sql
TEMPLATE = app

HEADERS = \
    NodeInit.h \

SOURCES = \
    NodeInit.cpp \
    schatd2.cpp \

DEFINES += SCHAT_DAEMON
SCHAT_DAEMON_LIB = 1

TRANSLATIONS += ../../res/translations/schatd2_en.ts
TRANSLATIONS += ../../res/translations/schatd2_ru.ts
CODECFORTR = UTF-8

unix:!macx {
  ssl.files = ../../misc/tools/server.crt
  ssl.files += ../../misc/tools/server.key
  ssl.path = $$SCHAT_PREFIX/etc/schatd2
  
  target.path += $$SCHAT_PREFIX/usr/sbin
  INSTALLS += target ssl
}

win32 {
  qt.files += $$[QT_INSTALL_PREFIX]/bin/libeay32.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/ssleay32.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/zlib1.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/msvcp100.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/msvcr100.dll 
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtCore4.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtNetwork4.dll
  qt.files += $$[QT_INSTALL_PREFIX]/bin/QtSql4.dll
  qt.path += ../../os/win32/schatd2/

  sqldrivers.files += $$[QT_INSTALL_PREFIX]/plugins/sqldrivers/qsqlite4.dll
  sqldrivers.path += ../../os/win32/schatd2/plugins/qt/sqldrivers

  doc.files += ../../res/doc/ChangeLog.html
  doc.path += ../../os/win32/schatd2/doc/

  ssl.files = ../../misc/tools/server.crt
  ssl.files += ../../misc/tools/server.key
  ssl.path  = ../../os/win32/schatd2/

  target.path += ../../os/win32/schatd2/
  INSTALLS += target qt sqldrivers doc ssl
}

include(../common/config.pri)
include(../common/common.pri)


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

SCHAT_RESOURCES   = 1
SCHAT_RC_FILE     = 1
SCHAT_SINGLEAPP   = 0
TARGET            = schat-authd
DEFINES          += SCHAT_DAEMON
SCHAT_DAEMON_LIB  = 1
SCHAT_TUFAO_LIB   = 1

QT = core network
TEMPLATE = app

DEPENDPATH  += ../schatd2
INCLUDEPATH += ../schatd2

unix:!macx { 
  css.files += ../../res/www/css/bootstrap.css
  css.files += ../../res/www/css/result.css
  css.path = /usr/share/schatd2/www/css

  img.files += ../../res/www/img/spinner.gif
  img.files += ../../res/www/img/logo.png
  img.path = /usr/share/schatd2/www/img

  js.files += ../../res/www/js/html5.js
  js.files += ../../res/www/js/result.js
  js.path = /usr/share/schatd2/www/js

  jquery.files += ../../res/html/js/jquery-1.7.2.min.js
  jquery.path = /usr/share/schatd2/www/js

  other.files += ../../res/www/favicon.ico
  other.path = /usr/share/schatd2/www

  target.path += $$SCHAT_PREFIX/usr/sbin
  INSTALLS += target css img js other jquery
}

win32 {
  css.files += ../../res/www/css/bootstrap.css
  css.files += ../../res/www/css/result.css
  css.path  = ../../os/win32/schatd2/www/css

  img.files += ../../res/www/img/spinner.gif
  img.files += ../../res/www/img/logo.png
  img.path = ../../os/win32/schatd2/www/img

  js.files += ../../res/www/js/html5.js
  js.files += ../../res/www/js/result.js
  js.path = ../../os/win32/schatd2/www/js

  jquery.files += ../../res/html/js/jquery-1.7.2.min.js
  jquery.path = ../../os/win32/schatd2/www/js

  other.files += ../../res/www/favicon.ico
  other.path = ../../os/win32/schatd2/www

  target.path += ../../os/win32/schatd2/
  INSTALLS += target css img js other jquery
}

HEADERS = \
    AuthCore.h \
    AuthHandler.h \
    AuthState.h \
    HandlerCreator.h \
    HandlerRoute.h \
    handlers.h \
    handlers/ProvidersHandler.h \
    handlers/StateHandler.h \
    oauth2/GoogleAuth.h \
    oauth2/GoogleAuthData.h \
    oauth2/OAuthData.h \
    oauth2/OAuthHandler.h \

SOURCES = \
    AuthCore.cpp \
    authd.cpp \
    AuthHandler.cpp \
    AuthState.cpp \
    HandlerCreator.cpp \
    HandlerRoute.cpp \
    handlers/ProvidersHandler.cpp \
    handlers/StateHandler.cpp \
    oauth2/GoogleAuth.cpp \
    oauth2/GoogleAuthData.cpp \
    oauth2/OAuthData.cpp \
    oauth2/OAuthHandler.cpp \

include(../common/config.pri)
include(../common/common.pri)


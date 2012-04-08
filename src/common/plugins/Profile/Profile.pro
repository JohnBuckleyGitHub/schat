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
QT = core gui network webkit

HEADERS  = \
   CountryField.h \
   ProfileChatView.h \
   ProfilePlugin.h \
   ProfilePlugin_p.h \

SOURCES  = \
   CountryField.cpp \
   ProfileChatView.cpp \
   ProfilePlugin.cpp \

RESOURCES += Profile.qrc

unix {
  translations.files = ../../../../res/translations/profile_en.qm
  translations.files += ../../../../res/translations/profile_ru.qm
  
  macx {
    translations.path += ../../../../out/SimpleChat2.app/Contents/Resources/translations
    target.path += ../../../../out/SimpleChat2.app/Contents/PlugIns/
  } else {
    translations.path = $$SCHAT_PREFIX/usr/share/schat2/translations
    target.path += $$SCHAT_PREFIX/usr/share/schat2/plugins
  }
  
  INSTALLS += target translations
}

TRANSLATIONS += ../../../../res/translations/profile_en.ts
TRANSLATIONS += ../../../../res/translations/profile_ru.ts
CODECFORTR = UTF-8

include(../plugins.pri)

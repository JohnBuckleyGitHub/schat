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

DEPENDPATH += 3rdparty
INCLUDEPATH += 3rdparty

HEADERS  = \
   SpellCheckerPlugin.h \
   SpellHighlighter.h \
   SpellBackend.h \
   HunSpellChecker.h \
   SpellChecker.h \
   SpellCheckerPage.h \

SOURCES  = \
   SpellCheckerPlugin.cpp \
   SpellHighlighter.cpp \
   SpellBackend.cpp \
   HunSpellChecker.cpp \
   SpellChecker.cpp \
   SpellCheckerPage.cpp \

RESOURCES += SpellChecker.qrc

include(3rdparty/hunspell/hunspell.pri)
include(../plugins.pri)

TRANSLATIONS += res/translations/spellchecker_en.ts
TRANSLATIONS += res/translations/spellchecker_ru.ts
CODECFORTR = UTF-8

macx:dictionaries.path += ../../../../out/SimpleChat2.app/Contents/Resources/spelling
win32:dictionaries.path += ../../../../os/win32/$${PLUGIN_TARGET}/spelling

win32|macx {
  dictionaries.files += spelling/en_US.aff
  dictionaries.files += spelling/en_US.dic
  dictionaries.files += spelling/ru_RU.aff
  dictionaries.files += spelling/ru_RU.dic
  
  INSTALLS += dictionaries
}

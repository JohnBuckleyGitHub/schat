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
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.

SCHAT_CONSOLE    = 0
SCHAT_DEBUG      = 0
SCHAT_RESOURCES  = 1
SCHAT_RC_FILE    = 1
SCHAT_STATIC     = 0
SCHAT_SINGLEAPP  = 0

QT += network
TEMPLATE = app

HEADERS += \
    3rdparty/qtwin.h \
    aboutdialog.h \
    abstractsettings.h \
    customizewizard.h \
    md5calcthread.h \
    mirrorwriter.h \
    page/intropage.h \
    page/nsispage.h \
    page/progresspage.h \
    page/selectpage.h \
    page/settingspage.h \
    translation.h \
    updatexmlreader.h \
    version.h \
    wizardsettings.h \

SOURCES += \
    3rdparty/qtwin.cpp \
    aboutdialog.cpp \
    abstractsettings.cpp \
    customizewizard.cpp \
    main.cpp \
    md5calcthread.cpp \
    mirrorwriter.cpp \
    page/intropage.cpp \
    page/nsispage.cpp \
    page/progresspage.cpp \
    page/selectpage.cpp \
    page/settingspage.cpp \
    translation.cpp \
    updatexmlreader.cpp \
    wizardsettings.cpp \

contains( SCHAT_STATIC, 1 ) {
    DEFINES += SCHAT_STATIC
}

DEFINES += SCHAT_NO_UPDATE_WIDGET

TRANSLATIONS += ../../data/translations/customize_en.ts
TRANSLATIONS += ../../data/translations/customize_ru.ts
TRANSLATIONS += ../../data/translations/customize_uk.ts
CODECFORTR = UTF-8

include(../common/common.pri)

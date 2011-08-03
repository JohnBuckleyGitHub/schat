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

SCHAT_RESOURCES   = 0
SCHAT_RC_FILE     = 1
SCHAT_SINGLEAPP   = 0

QT = core gui network webkit
TEMPLATE = app

HEADERS = \
    arora/lineedit.h \
    arora/lineedit_p.h \
    ChatApp.h \
    debugstream.h \
    QProgressIndicator/QProgressIndicator.h \
    ui/ChatWindow.h \
    ui/ColorButton.h \
    ui/fields/GenderField.h \
    ui/fields/LanguageField.h \
    ui/fields/NickEdit.h \
    ui/fields/ProfileField.h \
    ui/InputWidget.h \
    ui/LanguageBox.h \
    ui/NetworkWidget.h \
    ui/SendWidget.h \
    ui/SoundButton.h \
    ui/StatusBar.h \
    ui/StatusMenu.h \
    ui/StatusWidget.h \
    ui/TabBar.h \
    ui/tabs/AboutTab.h \
    ui/tabs/AlertTab.h \
    ui/tabs/ChannelTab.h \
    ui/tabs/ProgressTab.h \
    ui/tabs/SettingsTab.h \
    ui/tabs/SettingsTab_p.h \
    ui/tabs/UserView.h \
    ui/tabs/WelcomeTab.h \
    ui/TabWidget.h \
    ui/TrayIcon.h \

SOURCES = \
    arora/lineedit.cpp \
    ChatApp.cpp \
    main.cpp \
    QProgressIndicator/QProgressIndicator.cpp \
    ui/ChatWindow.cpp \
    ui/ColorButton.cpp \
    ui/fields/GenderField.cpp \
    ui/fields/LanguageField.cpp \
    ui/fields/NickEdit.cpp \
    ui/fields/ProfileField.cpp \
    ui/InputWidget.cpp \
    ui/LanguageBox.cpp \
    ui/NetworkWidget.cpp \
    ui/SendWidget.cpp \
    ui/SoundButton.cpp \
    ui/StatusBar.cpp \
    ui/StatusMenu.cpp \
    ui/StatusWidget.cpp \
    ui/TabBar.cpp \
    ui/tabs/AboutTab.cpp \
    ui/tabs/AlertTab.cpp \
    ui/tabs/ChannelTab.cpp \
    ui/tabs/ProgressTab.cpp \
    ui/tabs/SettingsTab.cpp \
    ui/tabs/UserView.cpp \
    ui/tabs/WelcomeTab.cpp \
    ui/TabWidget.cpp \
    ui/TrayIcon.cpp \
    
TRANSLATIONS += ../../data/translations/schat2_en.ts
TRANSLATIONS += ../../data/translations/schat2_ru.ts
CODECFORTR = UTF-8

win32 {
    HEADERS += qtwin/qtwin.h
    SOURCES += qtwin/qtwin.cpp
}

DEFINES += SCHAT_WEBKIT

SCHAT_CLIENT_LIB = 1
SCHAT_CORE_LIB = 1

include(../common/common.pri)


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

SCHAT_RESOURCES = 1
SCHAT_DEBUG     = 1
SCHAT_USE_SSL   = 1

TEMPLATE = lib
QT = core gui network webkit
TARGET = schat
DEFINES += SCHAT_CORE_LIBRARY
win32:RC_FILE = schat2-core.rc

HEADERS = \
    actions/ChatViewAction.h \
    actions/MenuBuilder.h \
    alerts/MessageAlert.h \
    arora/lineedit.h \
    arora/lineedit_p.h \
    ChatAlerts.h \
    ChatCore.h \
    ChatHooks.h \
    ChatNotify.h \
    ChatPlugins.h \
    ChatSettings.h \
    ChatUrls.h \
    feeds/AccountFeed.h \
    hooks/ChannelMenu.h \
    hooks/ChannelMenuImpl.h \
    hooks/ChannelsImpl.h \
    hooks/ChatViewHooks.h \
    hooks/ClientFeedsImpl.h \
    hooks/ClientImpl.h \
    hooks/CommandsImpl.h \
    hooks/MessagesImpl.h \
    hooks/RegCmds.h \
    hooks/UserMenuImpl.h \
    messages/ChannelMessage.h \
    messages/Message.h \
    messages/ServiceMessage.h \
    NetworkManager.h \
    plugins/ChatPlugin.h \
    QProgressIndicator/QProgressIndicator.h \
    schat.h \
    text/LinksFilter.h \
    text/TokenFilter.h \
    text/UrlFilter.h \
    ui/ChatIcons.h \
    ui/ColorButton.h \
    ui/fields/GenderField.h \
    ui/fields/LanguageField.h \
    ui/fields/NickEdit.h \
    ui/fields/ProfileField.h \
    ui/InputWidget.h \
    ui/LanguageBox.h \
    ui/LoginIcon.h \
    ui/MainToolBar.h \
    ui/network/AccountButton.h \
    ui/network/LoginWidget.h \
    ui/network/NetworkButton.h \
    ui/network/NetworkExtra.h \
    ui/network/NetworkWidget.h \
    ui/network/OfflineLogin.h \
    ui/network/Password.h \
    ui/network/SecurityQuestion.h \
    ui/network/SignUpWidget.h \
    ui/SendWidget.h \
    ui/SoundButton.h \
    ui/StatusBar.h \
    ui/StatusMenu.h \
    ui/StatusWidget.h \
    ui/TabBar.h \
    ui/tabs/AboutTab.h \
    ui/tabs/AbstractTab.h \
    ui/tabs/AlertTab.h \
    ui/tabs/ChannelBar.h \
    ui/tabs/ChannelBaseTab.h \
    ui/tabs/ChannelTab.h \
    ui/tabs/ChatView.h \
    ui/tabs/PrivateTab.h \
    ui/tabs/ProgressTab.h \
    ui/tabs/SettingsTab.h \
    ui/tabs/SettingsTab_p.h \
    ui/tabs/UserView.h \
    ui/tabs/WelcomeTab.h \
    ui/TabsToolBar.h \
    ui/TabWidget.h \
    ui/TrayIcon.h \

SOURCES = \
    actions/ChatViewAction.cpp \
    actions/MenuBuilder.cpp \
    alerts/MessageAlert.cpp \
    arora/lineedit.cpp \
    ChatAlerts.cpp \
    ChatCore.cpp \
    ChatHooks.cpp \
    ChatNotify.cpp \
    ChatPlugins.cpp \
    ChatSettings.cpp \
    ChatUrls.cpp \
    feeds/AccountFeed.cpp \
    hooks/ChannelMenu.cpp \
    hooks/ChannelMenuImpl.cpp \
    hooks/ChannelsImpl.cpp \
    hooks/ChatViewHooks.cpp \
    hooks/ClientFeedsImpl.cpp \
    hooks/ClientImpl.cpp \
    hooks/CommandsImpl.cpp \
    hooks/MessagesImpl.cpp \
    hooks/RegCmds.cpp \
    hooks/UserMenuImpl.cpp \
    messages/ChannelMessage.cpp \
    messages/Message.cpp \
    messages/ServiceMessage.cpp \
    NetworkManager.cpp \
    plugins/ChatPlugin.cpp \
    QProgressIndicator/QProgressIndicator.cpp \
    text/LinksFilter.cpp \
    text/TokenFilter.cpp \
    text/UrlFilter.cpp \
    ui/ChatIcons.cpp \
    ui/ColorButton.cpp \
    ui/fields/GenderField.cpp \
    ui/fields/LanguageField.cpp \
    ui/fields/NickEdit.cpp \
    ui/fields/ProfileField.cpp \
    ui/InputWidget.cpp \
    ui/LanguageBox.cpp \
    ui/LoginIcon.cpp \
    ui/MainToolBar.cpp \
    ui/network/AccountButton.cpp \
    ui/network/LoginWidget.cpp \
    ui/network/NetworkButton.cpp \
    ui/network/NetworkExtra.cpp \
    ui/network/NetworkWidget.cpp \
    ui/network/OfflineLogin.cpp \
    ui/network/Password.cpp \
    ui/network/SecurityQuestion.cpp \
    ui/network/SignUpWidget.cpp \
    ui/SendWidget.cpp \
    ui/SoundButton.cpp \
    ui/StatusBar.cpp \
    ui/StatusMenu.cpp \
    ui/StatusWidget.cpp \
    ui/TabBar.cpp \
    ui/tabs/AboutTab.cpp \
    ui/tabs/AbstractTab.cpp \
    ui/tabs/AlertTab.cpp \
    ui/tabs/ChannelBar.cpp \
    ui/tabs/ChannelBaseTab.cpp \
    ui/tabs/ChannelTab.cpp \
    ui/tabs/ChatView.cpp \
    ui/tabs/PrivateTab.cpp \
    ui/tabs/ProgressTab.cpp \
    ui/tabs/SettingsTab.cpp \
    ui/tabs/UserView.cpp \
    ui/tabs/WelcomeTab.cpp \
    ui/TabsToolBar.cpp \
    ui/TabWidget.cpp \
    ui/TrayIcon.cpp \

SCHAT_CLIENT_LIB = 1

TRANSLATIONS += ../../res/translations/schat2_en.ts
TRANSLATIONS += ../../res/translations/schat2_ru.ts
CODECFORTR = UTF-8

unix {
  macx {
    target.path += ../../out/SimpleChat2.app/Contents/Frameworks/
  } else {
    target.path += $$SCHAT_PREFIX/usr/lib
  }

  INSTALLS += target
}

include(../common/common.pri)

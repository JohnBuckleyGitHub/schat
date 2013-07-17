/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtPlugin>

#include "ChannelsCmd.h"
#include "ChannelsMenuImpl.h"
#include "ChannelsPlugin.h"
#include "ChannelsPlugin_p.h"
#include "ChannelsSettings.h"
#include "ChatCore.h"
#include "ChatSettings.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "hooks/ChatViewHooks.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Translation.h"
#include "ui/tabs/ChatView.h"

ChannelsPluginImpl::ChannelsPluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  new ChannelsCmd(this);
  new ChannelsMenuImpl(this);

  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatClient::channels(), SIGNAL(channel(QByteArray)), SLOT(channel(QByteArray)));

  ChatCore::translation()->addOther(LS("channels"));
  ChatCore::settings()->setDefault(LS("Channels/Ignoring"), false);

  connect(ChatViewHooks::i(), SIGNAL(initHook(ChatView*)), SLOT(init(ChatView*)));
  connect(ChatViewHooks::i(), SIGNAL(loadFinishedHook(ChatView*)), SLOT(loadFinished(ChatView*)));
}


/*!
 * Игнорирование пользователя.
 */
void ChannelsPluginImpl::ignore(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return;

  ClientFeeds::post(ChatClient::id(), LS("acl/head/other/") + SimpleID::encode(id), Acl::Read, Feed::Share | Feed::Broadcast);
}


/*!
 * Отмена игнорирования пользователя.
 */
void ChannelsPluginImpl::unignore(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::UserId)
    return;

  ClientFeeds::del(ChatClient::id(), LS("acl/head/other/") + SimpleID::encode(id), Feed::Share | Feed::Broadcast);
}


void ChannelsPluginImpl::channel(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return;

  FeedPtr feed = ChatClient::channels()->get(id)->feed(LS("acl"), false);
  if (!feed)
    ClientFeeds::request(id, LS("get"), LS("acl/head"));
}


/*!
 * Инициализация JavaScript.
 */
void ChannelsPluginImpl::init(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) != SimpleID::ChannelId)
    return;

  view->addJS(LS("qrc:/js/Channels/Channels.js"));
}


/*!
 * Инициализация CSS.
 */
void ChannelsPluginImpl::loadFinished(ChatView *view)
{
  if (SimpleID::typeOf(view->id()) != SimpleID::ChannelId)
    return;

  view->addCSS(LS("qrc:/css/Channels/Channels.css"));
}


void ChannelsPluginImpl::ready()
{
  FeedPtr feed = ChatClient::channel()->feed(LS("acl"), false);
  if (!feed)
    ClientFeeds::request(ChatClient::id(), LS("get"), LS("acl/head"));
}


ChatPlugin *ChannelsPlugin::create()
{
  m_plugin = new ChannelsPluginImpl(this);
  return m_plugin;
}


QWidget *ChannelsPlugin::settings(QWidget *parent)
{
  return new ChannelsSettings(parent);
}


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Channels, ChannelsPlugin);
#endif
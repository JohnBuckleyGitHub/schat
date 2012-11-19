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
#include "ChannelsPlugin.h"
#include "ChannelsPlugin_p.h"
#include "client/ChatClient.h"
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "net/SimpleID.h"
#include "sglobal.h"

ChannelsPluginImpl::ChannelsPluginImpl(QObject *parent)
  : ChatPlugin(parent)
{
  new ChannelsCmd(this);

  connect(ChatClient::i(), SIGNAL(ready()), SLOT(ready()));
  connect(ChatClient::channels(), SIGNAL(channel(QByteArray)), SLOT(channel(QByteArray)));
}


void ChannelsPluginImpl::channel(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return;

  FeedPtr feed = ChatClient::channels()->get(id)->feed(LS("acl"), false);
  if (!feed)
    ClientFeeds::request(id, LS("get"), LS("acl/head"));
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


#if QT_VERSION < 0x050000
  Q_EXPORT_PLUGIN2(Channels, ChannelsPlugin);
#endif

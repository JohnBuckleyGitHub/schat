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

#include "feeds/FeedStorage.h"
#include "GenericCh.h"
#include "sglobal.h"
#include "feeds/NodeLinksFeed.h"

GenericCh::GenericCh(QObject *parent)
  : Ch(parent)
{
}


void GenericCh::channelImpl(ChatChannel channel, ChatChannel /*user*/)
{
  channel->feed(LS("topic"));
}


void GenericCh::newChannelImpl(ChatChannel channel, ChatChannel user)
{
  addNewFeedIsNotExist(channel, LS("acl"), user);
}


void GenericCh::serverImpl(ChatChannel channel, bool /*created*/)
{
  channel->feed(LS("acl"));
}


void GenericCh::userChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host, bool /*created*/)
{
  if (!channel->account())
    channel->createAccount();

  channel->feed(LS("account"));

  addNewUserFeedIsNotExist(channel, LS("acl"));
  addNewUserFeedIsNotExist(channel, LS("links"));

  NodeLinksFeed::add(channel, data, host);
}

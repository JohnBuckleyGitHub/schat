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

#include "JSON.h"

GenericCh::GenericCh(QObject *parent)
  : Ch(parent)
{
}


void GenericCh::newChannelImpl(ChatChannel channel, ChatChannel user)
{
  if (channel->type() == SimpleID::ChannelId) {
    FeedPtr acl = channel->feed(LS("acl"), true, false);
    if (user)
      acl->head().acl().add(user->id());

    FeedStorage::save(acl);

    channel->feed(LS("topic"));
  }
  else if (channel->type() == SimpleID::ServerId) {
    channel->feed(LS("acl"));
  }
}


void GenericCh::newUserChannelImpl(ChatChannel channel, const AuthRequest &data, const QString &host, bool /*created*/)
{
  if (!channel->account())
    channel->createAccount();

  channel->feed(LS("account"));

  addNewUserFeedIsNotExist(channel, LS("acl"));
  addNewUserFeedIsNotExist(channel, LS("links"));

  NodeLinksFeed::add(channel, data, host);
}


/*!
 * Создание при необходимости пользовательского фида.
 */
void GenericCh::addNewUserFeedIsNotExist(ChatChannel channel, const QString &name)
{
  FeedPtr feed = channel->feed(name, false);
  if (feed)
    return;

  feed = channel->feed(name, true, false);
  feed->head().acl().add(channel->id());

  FeedStorage::save(feed);
}

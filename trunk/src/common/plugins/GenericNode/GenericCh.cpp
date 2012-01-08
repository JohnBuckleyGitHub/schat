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

GenericCh::GenericCh(QObject *parent)
  : Ch(parent)
{
}


bool GenericCh::addImpl(ChatChannel channel)
{
  if (channel->type() == SimpleID::ChannelId) {
    if (!channel->feeds().all().contains("acl"))
      channel->feeds().add(FeedStorage::create("acl"));

    if (!channel->feeds().all().contains("topic"))
      channel->feeds().add(FeedStorage::create("topic"));
  }

  return true;
}


void GenericCh::newUserChannelImpl(ChatChannel channel, const AuthRequest & /*data*/, const QString & /*host*/, bool /*created*/)
{
  if (!channel->account())
    channel->createAccount();

  if (!channel->feeds().all().contains("account"))
    channel->feeds().add(FeedStorage::create("account"));

  if (!channel->feeds().all().contains("acl")) {
    Feed *acl = FeedStorage::create("acl");
    acl->head().acl().add(channel->id());

    channel->feeds().add(acl);
  }
}

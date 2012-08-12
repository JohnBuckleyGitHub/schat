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

#include <QDebug>

#include "feeds/FeedStorage.h"
#include "GenericCh.h"
#include "sglobal.h"


GenericCh::GenericCh(QObject *parent)
  : ChHook(parent)
{
}


GenericCh2::GenericCh2(QObject *parent)
  : Ch(parent)
{
}


void GenericCh2::newChannelImpl(ChatChannel channel, ChatChannel user)
{
  addNewFeedIfNotExist(channel, LS("acl"), user);
}


void GenericCh2::serverImpl(ChatChannel channel, bool /*created*/)
{
  channel->feed(LS("acl"));
  channel->feed(LS("server"));
}


void GenericCh2::userChannelImpl(ChatChannel channel, const AuthRequest & /*data*/, const QString & /*host*/, bool /*created*/, quint64 /*socket*/)
{
  if (!channel->account())
    channel->createAccount();

  channel->feed(LS("account"));

  addNewUserFeedIfNotExist(channel, LS("acl"));
  addNewUserFeedIfNotExist(channel, LS("profile"));
}

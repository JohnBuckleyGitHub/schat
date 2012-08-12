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

#include "MessagesCh.h"
#include "sglobal.h"

MessagesCh::MessagesCh(QObject *parent)
  : ChHook(parent)
{
}


void MessagesCh::sync(ChatChannel channel, ChatChannel user)
{
  Q_UNUSED(user)
  Ch::addNewFeedIfNotExist(channel, LS("history"));
}


MessagesCh2::MessagesCh2(QObject *parent)
  : Ch(parent)
{
}



void MessagesCh2::newChannelImpl(ChatChannel channel, ChatChannel user)
{
  addNewFeedIfNotExist(channel, LS("history"), user);
}


void MessagesCh2::userChannelImpl(ChatChannel channel, const AuthRequest & /*data*/, const QString & /*host*/, bool /*created*/, quint64 /*socket*/)
{
  addNewUserFeedIfNotExist(channel, LS("history"));
}

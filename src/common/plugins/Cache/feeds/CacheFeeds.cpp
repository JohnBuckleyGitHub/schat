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

#include "client/ChatClient.h"
#include "client/ClientFeeds.h"
#include "feeds/CacheFeeds.h"
#include "net/packets/FeedNotice.h"

namespace Hooks
{

CacheFeeds::CacheFeeds(QObject *parent)
  : Feeds(parent)
{
  ChatClient::feeds()->hooks()->add(this);
}


void CacheFeeds::readFeedImpl(const FeedNotice &packet)
{
  qDebug() << " [Cache] CacheFeeds::readFeed()" << packet.command();
}

} // namespace Hooks

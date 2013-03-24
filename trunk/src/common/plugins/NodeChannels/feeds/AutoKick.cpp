/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "feeds/AutoKick.h"
#include "feeds/FeedEvents.h"
#include "feeds/FeedStrings.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"
#include "Ch.h"
#include "net/Channels.h"
#include "cores/Core.h"
#include "Sockets.h"
#include "net/packets/ChannelNotice.h"

AutoKick::AutoKick(QObject *parent)
  : QObject(parent)
{
  connect(FeedEvents::i(), SIGNAL(notify(FeedEvent)), SLOT(notify(FeedEvent)));
}


void AutoKick::notify(const FeedEvent &event)
{
  if (event.status != Notice::OK || event.name != FEED_NAME_ACL || event.method == FEED_METHOD_GET || SimpleID::typeOf(event.channel) != SimpleID::ChannelId)
    return;

  const QVariant value = event.request.value(FEED_KEY_VALUE);

  if (event.method == FEED_METHOD_POST && event.path.startsWith(LS("head/other/")) && value == 0) {
    kick(event.channel, SimpleID::decode(event.path.mid(11)));
  }
}


/*!
 * Отключение одиночного пользователя.
 */
void AutoKick::kick(const QByteArray &channelId, const QByteArray &userId)
{
  ChatChannel channel = Ch::channel(channelId, SimpleID::ChannelId, false);
  if (!channel)
    return;

  ChatChannel user = Ch::channel(channelId, SimpleID::UserId, false);
  if (!user)
    return;

  user->removeChannel(channelId);
  if (!channel->channels().all().contains(userId))
    return;

  Core::i()->send(Sockets::channel(channel), ChannelNotice::request(userId, channelId, CHANNELS_PART_CMD));
  channel->removeChannel(userId);

  Ch::gc(channel);
}

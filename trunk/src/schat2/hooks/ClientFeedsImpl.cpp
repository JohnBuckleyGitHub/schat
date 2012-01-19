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
#include "client/ClientChannels.h"
#include "client/ClientFeeds.h"
#include "feeds/FeedStorage.h"
#include "hooks/ClientFeedsImpl.h"
#include "net/packets/FeedNotice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

ClientFeedsImpl::ClientFeedsImpl(QObject *parent)
  : Feeds(parent)
{
  ChatClient::feeds()->hooks()->add(this);
}


void ClientFeedsImpl::readFeed(const FeedNotice &packet)
{
  m_channel = ChatClient::channels()->get(packet.sender());
  if (!m_channel)
    return;

  QString cmd = packet.command();
  if (cmd.isEmpty())
    return;

  m_packet = &packet;

  if (cmd == LS("feed"))
    feed();
}


void ClientFeedsImpl::feed()
{
  QString name = m_packet->text();
  if (name.isEmpty())
    return;

  FeedPtr feed = FeedPtr(FeedStorage::load(name, m_packet->json().value(name).toMap()));
  if (!feed)
    return;

  m_channel->feeds().add(feed);
}

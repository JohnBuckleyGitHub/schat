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
#include "FeedsImpl.h"
#include "net/packets/FeedPacket.h"
#include "RawFeedsMessage.h"
#include "RawFeedsPlugin_p.h"
#include "ui/TabWidget.h"

namespace Hooks
{

FeedsImpl::FeedsImpl(RawFeeds *parent)
  : Feeds(parent)
{
  ChatClient::feeds()->hooks()->add(this);
}


void FeedsImpl::readFeed(const FeedPacket &packet)
{
  qDebug() << "FeedsImpl::readFeed()";

  if (TabWidget::i()) {
    RawFeedsMessage message(packet);
    TabWidget::i()->add(message);
  }
}

} // namespace Hooks
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

#include "NodeChannelIndex.h"
#include "NodeNotify.h"
#include "net/SimpleID.h"
#include "Ch.h"

NodeChannelIndex::NodeChannelIndex(QObject *parent)
  : QObject(parent)
{
  connect(NodeNotify::i(), SIGNAL(notify(NotifyItem)), SLOT(notify(NotifyItem)));
}


void NodeChannelIndex::build()
{
  qDebug() << "*** BUILD ***";
  qDebug() << channels().size();
}


void NodeChannelIndex::notify(const NotifyItem &notify)
{
  if (notify.type() == NotifyItem::ChannelBonding) {
    if (SimpleID::typeOf(notify.param1().toByteArray()) == SimpleID::ChannelId)
      build();
  }
}


/*!
 * Получение списка обычных каналов, в которых есть пользователи.
 */
QList<ChatChannel> NodeChannelIndex::channels() const
{
  QList<ChatChannel> out;
  QList<QByteArray> all = Ch::server()->channels().all();

  foreach (const QByteArray &id, all) {
    if (SimpleID::typeOf(id) == SimpleID::ChannelId) {
      ChatChannel channel = Ch::channel(id, SimpleID::ChannelId, false);
      if (channel)
        qDebug() << channel->name() << channel->channels().size();

      if (channel && channel->channels().size())
        out.append(channel);
    }
  }

  return out;
}

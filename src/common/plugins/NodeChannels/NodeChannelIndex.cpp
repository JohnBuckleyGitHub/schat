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

#include "Ch.h"
#include "DateTime.h"
#include "net/SimpleID.h"
#include "NodeChannelIndex.h"
#include "NodeNotify.h"
#include "sglobal.h"

ChannelIndexData::ChannelIndexData(ChatChannel channel)
  : count(0)
  , name(channel->name())
{
  FeedPtr feed = channel->feed(LS("info"), false);
  if (!feed)
    return;

  id    = channel->id();
  count = channel->channels().size();
  title = feed->data().value(LS("title")).toMap().value(LS("text")).toString();
}


bool ChannelIndexData::isValid() const
{
  if (!count || title.isEmpty())
    return false;

  return true;
}


bool ChannelIndexData::operator<(const ChannelIndexData &other) const
{
  if (other.count == count)
    return name.toLower() < other.name.toLower();

  return other.count < count;
}


NodeChannelIndex::NodeChannelIndex(QObject *parent)
  : QObject(parent)
  , m_date(0)
{
  connect(NodeNotify::i(), SIGNAL(notify(NotifyItem)), SLOT(notify(NotifyItem)));
}


/*!
 * Создание списка каналов.
 */
void NodeChannelIndex::build()
{
  QList<ChatChannel> channels = this->channels();

  m_list.clear();
  foreach (ChatChannel channel, channels) {
    ChannelIndexData data(channel);
    if (data.isValid())
      m_list.append(data);
  }

  qSort(m_list);
  m_date = DateTime::utc();

  FeedPtr feed = Ch::server()->feed(LS("list"), false);
  if (feed)
    feed->put(LS("channels"), QVariantMap(), Ch::server().data());
}


/*!
 * Обработка уведомлений.
 */
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
      if (channel && channel->channels().size())
        out.append(channel);
    }
  }

  return out;
}

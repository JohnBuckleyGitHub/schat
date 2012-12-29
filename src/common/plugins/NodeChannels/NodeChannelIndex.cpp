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

#include <QTimer>

#include "Ch.h"
#include "DateTime.h"
#include "net/SimpleID.h"
#include "NodeChannelIndex.h"
#include "NodeNotify.h"
#include "sglobal.h"

ChannelIndexData::ChannelIndexData(ChatChannel channel)
  : permanent(channel->permanent())
  , count(0)
  , visibility(0)
  , name(channel->name())
{
  FeedPtr feed = channel->feed(LS("info"), false);
  if (!feed)
    return;

  visibility = feed->data().value(LS("visibility"), 0).toInt();
  if (visibility < 0)
    return;

  id    = channel->id();
  count = channel->channels().size();
  title = feed->data().value(LS("title")).toMap().value(LS("text")).toString();
}


bool ChannelIndexData::isValid() const
{
  if (permanent)
    return true;

  return count;
}


bool ChannelIndexData::operator<(const ChannelIndexData &other) const
{
  if (other.count == count)
    return name.toLower() < other.name.toLower();

  return other.count < count;
}


NodeChannelIndex::NodeChannelIndex(QObject *parent)
  : QObject(parent)
  , m_updated(false)
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
  m_date    = DateTime::utc();
  m_updated = true;

  FeedPtr feed = Ch::server()->feed(LS("list"), false);
  if (feed)
    feed->put(LS("channels"), QVariantMap(), Ch::server().data());
}


/*!
 * Обработка уведомлений.
 */
void NodeChannelIndex::notify(const NotifyItem &notify)
{
  if (notify.type() == NotifyItem::ChannelBonding && SimpleID::typeOf(notify.param1().toByteArray()) == SimpleID::ChannelId) {
    reload();
  }
  else if (notify.type() == NotifyItem::FeedModified && SimpleID::typeOf(notify.param1().toByteArray()) == SimpleID::ChannelId && notify.param2() == LS("info")) {
    reload();
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
        out.append(channel);
    }
  }

  return out;
}


void NodeChannelIndex::reload()
{
  if (!m_date) {
    build();
  }
  else if (m_updated) {
    QTimer::singleShot(0, this, SLOT(build()));
    m_updated = false;
  }
}

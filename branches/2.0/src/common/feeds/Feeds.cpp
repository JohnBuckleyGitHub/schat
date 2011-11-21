/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2011 IMPOMEZIA <schat@impomezia.com>
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
#include "SimpleJSon.h"

#include "feeds/Feeds.h"
#include "feeds/FeedFactory.h"

bool Feeds::add(FeedPtr feed)
{
  if (!m_channel)
    return false;

  if (!feed)
    return false;

  feed->h().setChannel(m_channel);

  if (!feed->isValid())
    return false;

  m_feeds[feed->h().name()] = feed;
  return true;
}


/*!
 * Загрузка фидов из JSON данных.
 *
 * \param data JSON данные.
 */
void Feeds::load(const QVariantMap &data)
{
  qDebug() << "LOAD" << data.size();

  if (data.isEmpty())
    return;

  QMapIterator<QString, QVariant> i(data);
  while (i.hasNext()) {
    i.next();
    add(FeedFactory::load(i.key(), i.value().toMap()));
  }

  qDebug() << SimpleJSon::generate(save());
}


/*!
 * Получение фидов для одиночного пользователя.
 *
 * \param channel Канал-пользователь, используется для проверки прав, пользователь не получит фиды если у него нет прав на чтение.
 * \param feeds   Список имён фидов, если список пуст будут получены все фиды.
 * \param body    false если не нужно получать тело фидов.
 *
 * \sa headers().
 */
QVariantMap Feeds::get(Channel *channel, const QStringList &feeds, bool body) const
{
  if (feeds.isEmpty())
    return headers(channel, m_feeds.keys());

  QVariantMap json;

  for (int i = 0; i < feeds.size(); ++i) {
    FeedPtr feed = m_feeds.value(feeds.at(i));
    if (!feed)
      continue;

    QVariantMap header = feed->h().get(channel);
    if (header.isEmpty())
      continue;

    QVariantMap data;
    if (body)
      data = feed->get(channel);

    Feed::merge(data, header);
    merge(feeds.at(i), json, data);
  }

  return merge("feeds", json);
}


/*!
 * Получение заголовков фидов для одиночного пользователя.
 *
 * \param channel Канал-пользователь, используется для проверки прав, пользователь не получит список фидов если у него нет прав на чтение.
 * \param feeds   Список имён фидов, если список пуст будут получены заголовки всех доступных фидов.
 *
 * \sa get().
 */
QVariantMap Feeds::headers(Channel *channel, const QStringList &feeds) const
{
  return get(channel, feeds, false);
}


/*!
 * Получение JSON данных фидов для сохранения в базе данных.
 */
QVariantMap Feeds::save() const
{
  QVariantMap json;

  QMapIterator<QString, FeedPtr> i(m_feeds);
  while (i.hasNext()) {
    i.next();
    json[i.key()] = i.value()->save();
  }

  return merge("feeds", json);
}


bool Feeds::merge(const QString &key, QVariantMap &out, const QVariantMap &in)
{
  if (in.isEmpty())
    return false;

  out[key] = in;
  return true;
}


QVariantMap Feeds::merge(const QString &key, const QVariantMap &in)
{
  QVariantMap out;
  if (!in.isEmpty())
    out[key] = in;

  return out;
}

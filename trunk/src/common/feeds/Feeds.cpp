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

  feed->setChannel(m_channel);

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
 * Получение тела фида.
 *
 * \param name    Имя фида.
 * \param channel Указатель на канал для проверки прав доступа.
 *
 * \return Тело фида или пустые данные, если произошла ошибка.
 */
QVariantMap Feeds::feed(const QString &name, Channel *channel)
{
  QVariantMap json;
  if (name.isEmpty())
    return json;

  if (!m_feeds.contains(name))
    return json;

  FeedPtr feed = m_feeds.value(name);
  if (!feed)
    return json;

  QVariantMap header = feed->h().get(channel);
  if (header.isEmpty())
    return json;

  QVariantMap body = feed->get(channel);
  Feed::merge(body, header);

  merge(name, json, body);
  return json;
}


/*!
 * Получение заголовков фидов для одиночного пользователя.
 *
 * \param channel Канал-пользователь, используется для проверки прав, пользователь не получит список фидов если у него нет прав на чтение.
 * \param feeds   Список имён фидов, если список пуст будут получены заголовки всех доступных фидов.
 *
 * \sa get().
 */
QVariantMap Feeds::headers(Channel *channel) const
{
  QVariantMap json;

  QMapIterator<QString, FeedPtr> i(m_feeds);
  while (i.hasNext()) {
    i.next();
    merge(i.key(), json, i.value()->h().get(channel));
  }

  return merge("feeds", json);
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

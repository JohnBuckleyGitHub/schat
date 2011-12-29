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
#include "JSON.h"

#include "feeds/FeedFactory.h"
#include "feeds/Feeds.h"
#include "net/packets/Notice.h"

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


int Feeds::add(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (name.isEmpty())
    return Notice::BadRequest;

  if (m_feeds.contains(name))
    return Notice::ObjectAlreadyExists;

  if (!add(FeedFactory::create(name)))
    return Notice::InternalError;

  update(name, json, channel);
  return Notice::OK;
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

  qDebug() << JSON::generate(save());
}


FeedQueryReply Feeds::query(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (!m_feeds.contains(name))
    return FeedQueryReply(Notice::NotFound);

  return m_feeds.value(name)->query(json, channel);
}


int Feeds::clear(const QString &name, Channel *channel)
{
  if (!m_feeds.contains(name))
    return Notice::NotFound;

  return m_feeds.value(name)->clear(channel);
}


int Feeds::remove(const QString &name, Channel *channel)
{
  Q_UNUSED(channel)

  if (!m_feeds.contains(name))
    return Notice::NotFound;

  m_feeds.remove(name);
  return Notice::OK;
}


int Feeds::update(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (!m_feeds.contains(name))
    return Notice::NotFound;

  return m_feeds.value(name)->update(json, channel);
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
  if (name.isEmpty())
    return QVariantMap();

  if (!m_feeds.contains(name))
    return QVariantMap();

  FeedPtr feed = m_feeds.value(name);
  if (!feed)
    return QVariantMap();

  return Feed::merge(name, feed->feed(channel));
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
    Feed::merge(i.key(), json, i.value()->h().get(channel));
  }

  return Feed::merge("feeds", json);
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
    Feed::merge(i.key(), json, i.value()->save());
  }

  return Feed::merge("feeds", json);
}

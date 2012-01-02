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
#include "JSON.h"

#include "feeds/Feeds.h"
#include "feeds/FeedStorage.h"
#include "net/packets/Notice.h"

/*!
 * Добавление нового фида.
 */
bool Feeds::add(FeedPtr feed, bool save)
{
  if (!m_channel)
    return false;

  if (!feed)
    return false;

  feed->setChannel(m_channel);

  if (!feed->isValid())
    return false;

  m_feeds[feed->h().name()] = feed;

  if (save)
    FeedStorage::save(feed);

  return true;
}


int Feeds::add(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (name.isEmpty())
    return Notice::BadRequest;

  if (m_feeds.contains(name))
    return Notice::ObjectAlreadyExists;

  if (!add(FeedStorage::create(name), false))
    return Notice::InternalError;

  update(name, json, channel);
  return Notice::OK;
}


FeedQueryReply Feeds::query(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (!m_feeds.contains(name))
    return FeedQueryReply(Notice::NotFound);

  FeedQueryReply reply = m_feeds.value(name)->query(json, channel);
  if (reply.modified)
    FeedStorage::save(m_feeds.value(name));

  return reply;
}


int Feeds::clear(const QString &name, Channel *channel)
{
  if (!m_feeds.contains(name))
    return Notice::NotFound;

  int status = m_feeds.value(name)->clear(channel);
  if (status == Notice::OK)
    FeedStorage::save(m_feeds.value(name));

  return status;
}


int Feeds::remove(const QString &name, Channel *channel)
{
  Q_UNUSED(channel)

  if (!m_feeds.contains(name))
    return Notice::NotFound;

  m_feeds.remove(name);
  return Notice::OK;
}


/*!
 * Обновление данных фида, вызванное пользователем.
 *
 * \param name    ///< Имя фида.
 * \param json    ///< Новые данные фида.
 * \param channel ///< Канал для проверки привилегий.
 */
int Feeds::update(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (!m_feeds.contains(name))
    return Notice::NotFound;

  int status = m_feeds.value(name)->update(json, channel);
  if (status != Notice::OK)
    return status;

  return FeedStorage::save(m_feeds.value(name));
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

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
 * Базовая функция добавления нового фида.
 *
 * \param feed Указатель на фид.
 * \param save \b true если необходимо сохранить фид как новую ревизию.
 *
 * \return \b true если фид был добавлен.
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

  m_feeds[feed->head().name()] = feed;

  if (save)
    FeedStorage::save(feed);

  return true;
}


/*!
 * Обработка запроса пользователя к данным фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed query <имя фида> <опциональные JSON данные запроса>.
 *
 * \param name    Имя фида.
 * \param json    Данные запроса.
 * \param channel Указатель на канал-пользователь для проверки прав доступа.
 */
FeedQueryReply Feeds::query(const QString &name, const QVariantMap &json, Channel *channel)
{
  if (name.isEmpty())
    return Notice::BadRequest;

  if (!m_feeds.contains(name))
    return FeedQueryReply(Notice::NotFound);

  FeedQueryReply reply = m_feeds.value(name)->query(json, channel);
  if (reply.modified)
    FeedStorage::save(m_feeds.value(name));

  return reply;
}


/*!
 * Обработка запроса пользователя на очистку данных фида.
 *
 * В случае использования плагина "Raw Feeds" эта функция вызывается командой:
 * /feed clear <имя фида>.
 *
 * \param name    Имя фида.
 * \param channel Указатель на канал-пользователь для проверки прав доступа.
 */
int Feeds::clear(const QString &name, Channel *channel)
{
  if (!m_feeds.contains(name))
    return Notice::NotFound;

  int status = m_feeds.value(name)->clear(channel);
  if (status == Notice::OK)
    FeedStorage::save(m_feeds.value(name));

  return status;
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
 */
QVariantMap Feeds::headers(Channel *channel) const
{
  QVariantMap json;

  QMapIterator<QString, FeedPtr> i(m_feeds);
  while (i.hasNext()) {
    i.next();
    Feed::merge(i.key(), json, i.value()->head().get(channel));
  }

  return Feed::merge("feeds", json);
}

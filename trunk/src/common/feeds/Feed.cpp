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

#include "DateTime.h"
#include "feeds/Feed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"
#include "sglobal.h"

/*!
 * Создание фида на основе JSON данных.
 *
 * \param name Имя фида.
 * \param data JSON данные.
 */
Feed::Feed(const QString &name, const QVariantMap &data)
{
  m_header.setName(name);
  m_header.setData(data.value(LS("head")).toMap());
  m_data = data;
  m_data.remove(LS("head"));
}


/*!
 * Создание пустого именованного фида.
 */
Feed::Feed(const QString &name, qint64 date)
{
  m_header.setName(name);
  m_header.data()[LS("date")] = date;
}


/*!
 * Возвращает \b true если данные фида корректны.
 */
bool Feed::isValid() const
{
  return m_header.isValid();
}


Feed* Feed::create(const QString &name)
{
  return new Feed(name, DateTime::utc());
}


/*!
 * Создание фида на основе сохранённых JSON данных.
 *
 * \param name Имя фида.
 * \param data JSON данные.
 */
Feed* Feed::load(const QString &name, const QVariantMap &data)
{
  return new Feed(name, data);
}


FeedQueryReply Feed::query(const QVariantMap &json, Channel *channel)
{
  if (!channel)
    return FeedQueryReply(Notice::BadRequest);

  QString action = json.value(LS("action")).toString();
  if (action.isEmpty())
    return FeedQueryReply(Notice::BadRequest);

  if (!action.startsWith(LS("x-")))
    return FeedQueryReply(Notice::ServiceUnavailable);

  if (action == LS("x-mask"))
    return mask(json, channel);

  return FeedQueryReply(Notice::NotImplemented);
}


/*!
 * Обработка \b delete запроса к фиду.
 */
FeedReply Feed::del(const QString &path, Channel *channel)
{
  Q_UNUSED(channel)

  if (path.isEmpty())
    return Notice::BadRequest;

  if (path == LS("*")) {
    if (m_data.isEmpty())
      return Notice::NotModified;

    m_data.clear();
    return FeedReply(Notice::OK, DateTime::utc());
  }

  if (!m_data.contains(path))
    return Notice::NotFound;

  m_data.remove(path);
  return FeedReply(Notice::OK, DateTime::utc());
}


/*!
 * Обработка \b get запроса к фиду.
 *
 * \param path Путь запроса.
 * \param json Опциональные JSON данные запроса.
 * \param channel Пользователь совершивший запрос, если такой есть, функция не проверяет правда доступа.
 */
FeedReply Feed::get(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(json)
  Q_UNUSED(channel)

  if (path.isEmpty())
    return Notice::BadRequest;

  if (!m_data.contains(path))
    return Notice::NotFound;

  return FeedReply(Notice::OK, head().date());
}


/*!
 * Обработка \b post запроса к фиду.
 */
FeedReply Feed::post(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(channel)

  if (path.isEmpty() || path.contains(LC('*')) || !json.contains(LS("value")))
    return Notice::BadRequest;

  const QVariant& value = json[LS("value")];

  if (!m_data.contains(path) || m_data.value(path) != value) {
    m_data[path] = value;
    FeedReply reply(Notice::OK);
    reply.date = DateTime::utc();

    if (json.value(LS("options")).toInt() & Echo)
      reply.json[LS("value")] = value;

    return reply;
  }

  return Notice::NotModified;
}


/*!
 * Обработка \b put запроса к фиду.
 */
FeedReply Feed::put(const QString &path, const QVariantMap &json, Channel *channel)
{
  Q_UNUSED(channel)

  if (path.isEmpty() || !json.contains(LS("value")))
    return Notice::BadRequest;

  if (!m_data.contains(path))
    return Notice::NotFound;

  const QVariant& value = json[LS("value")];

  if (m_data.value(path) != value) {
    m_data[path] = value;
    FeedReply reply(Notice::OK);
    reply.date = DateTime::utc();

    if (json.value(LS("options")).toInt() & Echo)
      reply.json[LS("value")] = value;

    return reply;
  }

  return Notice::NotModified;
}


/*!
 * Получение тела фида.
 *
 * \param channel пользователь запросивший фид.
 */
QVariantMap Feed::feed(Channel *channel)
{
  Q_UNUSED(channel)

  return m_data;
}


/*!
 * Получение полных JSON данных фида для сохранения в хранилище.
 */
QVariantMap Feed::save()
{
  merge(LS("head"), m_data, m_header.save());
  return m_data;
}


void Feed::setChannel(Channel *channel)
{
  m_header.setChannel(channel);
}


bool Feed::merge(const QString &key, QVariantMap &out, const QVariantMap &in)
{
  if (in.isEmpty())
    return false;

  out[key] = in;
  return true;
}


QVariantMap Feed::merge(const QString &key, const QVariantMap &in)
{
  QVariantMap out;
  if (!in.isEmpty())
    out[key] = in;

  return out;
}


/*!
 * Слияние данных фидов.
 */
void Feed::merge(QVariantMap &out, const QVariantMap &in)
{
  QMapIterator<QString, QVariant> i(in);
  while (i.hasNext()) {
    i.next();
    out[i.key()] = i.value();
  }
}


/*!
 * Установка маски прав доступа к фиду.
 * Эта операция требует прав на редактирование.
 * Новая маска содержится в поле \b mask запроса \p json.
 *
 * \param json    Тело запроса.
 * \param channel Канал для проверки прав доступа.
 */
FeedQueryReply Feed::mask(const QVariantMap &json, Channel *channel)
{
  if (!Acl::canEdit(this, channel))
    return FeedQueryReply(Notice::Forbidden);

  if (!json.contains(LS("mask")))
    return FeedQueryReply(Notice::BadRequest);

  head().acl().setMask(json.value(LS("mask")).toInt());
  FeedQueryReply reply(Notice::OK);
  reply.modified = true;
  return reply;
}

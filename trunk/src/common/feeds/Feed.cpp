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

#include "DateTime.h"
#include "feeds/Feed.h"
#include "net/packets/Notice.h"
#include "net/SimpleID.h"

/*!
 * Создание пустого фида.
 */
Feed::Feed(qint64 date)
{
  m_header.setDate(date);
}


/*!
 * Создание фида на основе JSON данных.
 *
 * \param name Имя фида.
 * \param data JSON данные.
 */
Feed::Feed(const QString &name, const QVariantMap &data)
{
  m_header.setName(name);
  m_header.setData(data);
  m_data = data;
}


/*!
 * Создание пустого именованного фида.
 */
Feed::Feed(const QString &name, qint64 date)
{
  m_header.setName(name);
  m_header.setDate(date);
}


/*!
 * Возвращает \b true если данные фида корректны.
 */
bool Feed::isValid() const
{
  return m_header.isValid();
}


/*!
 * Создание фида на основе сохранённых JSON данных.
 *
 * \param name Имя фида.
 * \param data JSON данные.
 */
Feed* Feed::load(const QString &name, const QVariantMap &data)
{
  Q_UNUSED(data)

  return new Feed(name, data);
}


int Feed::clear(Channel *channel)
{
  if (!(m_header.acl().match(channel) & Acl::Write))
    return Notice::Forbidden;

  m_data.clear();
  m_header.setDate(DateTime::utc());

  return Notice::OK;
}


int Feed::update(const QVariantMap &json, Channel *channel)
{
  if (!(m_header.acl().match(channel) & Acl::Write))
    return Notice::Forbidden;

  merge(m_data, json);
  m_header.setDate(DateTime::utc());

  return Notice::OK;
}


QVariantMap Feed::feed(Channel *channel)
{
  QVariantMap header = m_header.get(channel);
  if (header.isEmpty())
    return QVariantMap();

  QVariantMap json = m_data;
  merge(json, header);
  return json;
}


/*!
 * Получение JSON данных фида для сохранения в базе данных.
 */
QVariantMap Feed::save() const
{
  QVariantMap out = m_data;
  merge(out, m_header.save());
  return out;
}


void Feed::setChannel(Channel *channel)
{
  m_header.setChannel(channel);
}


bool Feed::canRead(Channel *channel) const
{
  return m_header.acl().match(channel) & Acl::Read;
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


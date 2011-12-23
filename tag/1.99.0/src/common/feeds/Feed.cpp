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

#include "feeds/Feed.h"
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


QVariantMap Feed::get(Channel *channel) const
{
  if (canRead(channel))
    return m_data;

  return QVariantMap();
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


void Feed::merge(QVariantMap &out, const QVariantMap &in)
{
  QMapIterator<QString, QVariant> i(in);
  while (i.hasNext()) {
    i.next();
    out[i.key()] = i.value();
  }
}


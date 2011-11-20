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

#include "feeds/FeedHeader.h"
#include "net/SimpleID.h"

/*!
 * Конструктор создающий пустой объект.
 */
FeedHeader::FeedHeader()
  : m_channel(0)
  , m_date(0)
{
}


bool FeedHeader::isValid() const
{
  if (m_name.isEmpty())
    return false;

  if (!m_channel)
    return false;

  return true;
}


bool FeedHeader::json(QVariantMap &out, Channel *channel) const
{
  int acl = m_acl.acl();
  if (channel) {
    acl = m_acl.match(channel);
    if (!acl)
      return false;
  }

  out["acl"]  = acl;
  out["date"] = m_date;

  return true;
}


/*!
 * Получение заголовка фида в JSON формате.
 *
 * \return JSON данные или пустые данные, если фид не доступен для данного пользователя.
 */
QVariantMap FeedHeader::json(Channel *channel) const
{
  QVariantMap out;
  json(out, channel);
  return out;
}


/*!
 * Установка данных заголовка фида на основе JSON данных.
 */
void FeedHeader::setData(const QVariantMap &data)
{
  m_acl.setAcl(data["acl"].toLongLong());
  m_date = data["date"].toLongLong();
}

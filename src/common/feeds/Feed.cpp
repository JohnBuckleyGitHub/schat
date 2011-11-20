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

#include <QDateTime>

#include "feeds/Feed.h"
#include "net/SimpleID.h"


/*!
 * Конструктор создающий пустой объект.
 */
FeedHeader::FeedHeader()
  : m_date(0)
{
}


bool FeedHeader::isValid() const
{
  if (m_name.isEmpty())
    return false;

  if (SimpleID::typeOf(m_id) != SimpleID::ChannelId)
    return false;

  return true;
}


bool FeedHeader::json(QVariantMap &out, ClientUser user) const
{
  int acl = m_acl.acl();
  if (user) {
    acl = m_acl.match(user);
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
QVariantMap FeedHeader::json(ClientUser user) const
{
  QVariantMap out;
  json(out, user);
  return out;
}


Feed::Feed()
{
}


Feed::Feed(const QString &name, qint64 date)
{
  m_header.setName(name);
  m_header.setDate(date);
  m_data["example"] = true;
}


/*!
 * Возвращает \b true если данные фида корректны.
 */
bool Feed::isValid() const
{
  return m_header.isValid();
}

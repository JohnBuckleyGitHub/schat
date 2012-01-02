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

#include "feeds/FeedHeader.h"
#include "net/SimpleID.h"

/*!
 * Конструктор создающий пустой объект.
 */
FeedHeader::FeedHeader()
  : m_channel(0)
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


QVariantMap FeedHeader::get(Channel *channel)
{
  int acl = m_acl.match(channel);

  if (!(acl & Acl::Read))
    return QVariantMap();

  m_data["acl"] = acl;
  return m_data;
}


QVariantMap FeedHeader::save()
{
  m_data["acl"] = m_acl.acl();
  return m_data;
}


/*!
 * Установка данных заголовка фида на основе JSON данных.
 */
void FeedHeader::setData(const QVariantMap &data)
{
  m_acl.setAcl(data["acl"].toLongLong());
  m_data["date"] = data.value("date");
}

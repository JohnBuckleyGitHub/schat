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

#include "Account.h"
#include "acl/Acl.h"
#include "Channel.h"

Acl::Acl(int mask)
  : m_mask(mask)
{
}


bool Acl::can(Channel *channel, ResultAcl acl) const
{
  int r = match(channel);
  return r & acl;
}


/*!
 * Возвращает права доступа для конкретного пользователя.
 *
 * \param Канал-пользователь.
 * \return Acl::ResultAcl.
 */
int Acl::match(Channel *channel) const
{
  if (!channel)
    return (m_mask & ~0770);

  if (channel->account()->groups().all().contains("master"))
    return Read | Write | Edit;

  if (m_others.contains(channel->id()))
    return m_others.value(channel->id());

  if (m_owners.contains(channel->id()))
    return (m_mask >> 6);

  foreach (QString group, m_groups.all()) {
    if (channel->account()->groups().all().contains(group))
      return ((m_mask & ~0700) >> 3);
  }

  return (m_mask & ~0770);
}


/*!
 * Получение данных о правах доступа.
 */
QVariantMap Acl::get(Channel *channel)
{
  int acl = match(channel);

  if (!(acl & Read))
    return QVariantMap();

  QVariantMap json;
  if (acl & Edit)
    json = save();

  json["math"] = acl;
  json["mask"] = m_mask;
  return json;
}


QVariantMap Acl::save()
{
  QVariantMap json;
  json["mask"] = m_mask;

  return json;
}


void Acl::load(const QVariantMap &json)
{
  m_mask = json.value("mask").toInt();
}

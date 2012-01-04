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

#include "acl/Acl.h"

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
    return 06;

  return 06;
}


QVariantMap Acl::get(Channel *channel)
{
  int acl = match(channel);

  if (!(acl & Read))
    return QVariantMap();

  QVariantMap json;
  if (acl & Edit)
    json = save();

  json["math"] = acl;
  return json;
}


QVariantMap Acl::save()
{
  return QVariantMap();
}


void Acl::load(const QVariantMap &json)
{

}

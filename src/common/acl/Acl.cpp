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
#include "net/SimpleID.h"
#include "sglobal.h"

Acl::Acl(int mask)
  : m_mask(mask)
  , m_math(0)
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
 * \param channel Канал-пользователь.
 * \return Acl::ResultAcl.
 */
int Acl::match(Channel *channel) const
{
  if (!channel)
    return (m_mask & ~0770);

  if (channel->type() == SimpleID::ServerId)
    return Read | Write | Edit;

  if (channel->account()->groups.contains(LS("master")))
    return Read | Write | Edit;

  if (m_others.contains(channel->id()))
    return m_others.value(channel->id());

  if (m_owners.contains(channel->id()))
    return (m_mask >> 6);

  foreach (QString group, m_groups.all()) {
    if (channel->account()->groups.contains(group))
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

  json[LS("math")] = acl;
  json[LS("mask")] = m_mask;
  return json;
}


/*!
 * Сохранение информации о правах доступа.
 */
QVariantMap Acl::save()
{
  QVariantMap json;

  QVariantList owners;
  foreach (QByteArray owner, m_owners) {
    owners.append(SimpleID::encode(owner));
  }

  json[LS("mask")]   = m_mask;
  json[LS("owners")] = owners;

  if (m_math)
    json[LS("math")] = m_math;

  return json;
}


bool Acl::canEdit(Feed *feed, Channel *channel)
{
  return feed->head().acl().can(channel, Edit);
}


bool Acl::canRead(Feed *feed, Channel *channel)
{
  return feed->head().acl().can(channel, Read);
}


bool Acl::canWrite(Feed *feed, Channel *channel)
{
  return feed->head().acl().can(channel, Write);
}


void Acl::add(const QByteArray &owner)
{
  if (SimpleID::typeOf(owner) != SimpleID::UserId)
    return;

  if (m_owners.contains(owner))
    return;

  m_owners.append(owner);
}


void Acl::load(const QVariantMap &json)
{
  m_mask = json.value(LS("mask")).toInt();
  m_math = json.value(LS("math")).toInt();

  QVariantList owners = json.value("owners").toList();
  foreach (QVariant owner, owners) {
    add(SimpleID::decode(owner.toByteArray()));
  }
}

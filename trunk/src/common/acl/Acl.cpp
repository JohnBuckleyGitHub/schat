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
{
}


bool Acl::add(const QByteArray &other, int acl)
{
  if (SimpleID::typeOf(other) != SimpleID::UserId || acl < 0 || acl > 7)
    return false;

  m_others[other] = acl;
  return true;
}


bool Acl::can(Channel *channel, ResultAcl acl) const
{
  int r = match(channel);
  return r & acl;
}


/*!
 * Получение данных о правах доступа.
 */
bool Acl::get(QVariantMap &data, Channel *channel) const
{
  int acl = match(channel);

  if (!(acl & Read))
    return false;

  if (acl & Edit)
    save(data);

  data[LS("math")] = acl;
  data[LS("mask")] = m_mask;
  return true;
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

  foreach (const QString &group, m_groups.all()) {
    if (channel->account()->groups.contains(group))
      return ((m_mask & ~0700) >> 3);
  }

  return (m_mask & ~0770);
}


bool Acl::canEdit(const Feed *feed, Channel *channel)
{
  return feed->head().acl().can(channel, Edit);
}


bool Acl::canRead(const Feed *feed, Channel *channel)
{
  return feed->head().acl().can(channel, Read);
}


bool Acl::canWrite(const Feed *feed, Channel *channel)
{
  return feed->head().acl().can(channel, Write);
}


void Acl::add(const QByteArray &owner)
{
  if (SimpleID::typeOf(owner) != SimpleID::UserId || m_owners.contains(owner))
    return;

  m_owners.append(owner);
}


void Acl::load(const QVariantMap &json)
{
  m_mask = json.value(LS("mask")).toInt();

  const QVariantList owners = json.value(LS("owners")).toList();
  foreach (const QVariant &owner, owners)
    add(SimpleID::decode(owner.toByteArray()));

  const QVariantMap others = json.value(LS("others")).toMap();
  QMapIterator<QString, QVariant> i(others);
  while (i.hasNext()) {
    i.next();
    add(SimpleID::decode(i.key()), i.value().toInt());
  }

  m_groups = json.value(LS("groups")).toString();
}


/*!
 * Сохранение информации о правах доступа.
 */
void Acl::save(QVariantMap &data) const
{
  QVariantList owners;
  foreach (const QByteArray &owner, m_owners)
    owners.append(SimpleID::encode(owner));

  QVariantMap others;
  QMapIterator<QByteArray, int> i(m_others);
  while (i.hasNext()) {
    i.next();
    others[SimpleID::encode(i.key())] = i.value();
  }

  data[LS("mask")]   = m_mask;
  data[LS("owners")] = owners;
  data[LS("others")] = others;
  data[LS("groups")] = m_groups.toString();
}

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

#include <QCryptographicHash>

#include "net/ServerData.h"
#include "net/SimpleID.h"

ServerData::ServerData()
  : m_features(NoFeatures)
  , m_number(0)
{
}


bool ServerData::setChannelId(const QByteArray &id)
{
  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return false;

  m_channelId = id;
  m_features |= AutoJoinSupport;
  return true;
}


bool ServerData::setName(const QString &name)
{
  QString tmp = name.simplified().left(MaxNameLength);
  if (tmp.size() < MinNameLengh)
    return false;

  m_name = tmp;
  return true;
}


void ServerData::setPrivateId(const QByteArray &id)
{
  m_privateId = id;
  m_id = QCryptographicHash::hash(id, QCryptographicHash::Sha1) += SimpleID::ServerId;
}

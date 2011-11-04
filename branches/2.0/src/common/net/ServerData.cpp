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


/*!
 * Установка имени сервера.
 *
 * Имя сервера должно удовлетворять FQDN (Fully Qualified Domain Name),
 * однако имя, заканчивающееся на точку, будет признано некорректным.
 * Также имя не может начинаться с точки и содержать две точки подряд,
 * длина суффикса домена должна быть минимум 2 символа, например example.ru
 * или example.com корректные имена, а example.r некорректное.
 *
 * В случае отсутствия имени сервера регистрация будет не доступна.
 *
 * \return false если произошла ошибка.
 */
bool ServerData::setName(const QString &name)
{
  QString tmp = name.simplified().left(MaxNameLength);

  if (tmp.size() < MinNameLengh)
    return false;

  if (tmp.indexOf("..") != -1)
    return false;

  if (tmp.startsWith('.'))
    return false;

  if (tmp.endsWith('.'))
    return false;

  int index = tmp.lastIndexOf('.');
  if (index == -1 || tmp.size() - index < 3)
    return false;

  m_features |= PasswordAuthSupport;
  m_name = tmp;
  return true;
}


void ServerData::setPrivateId(const QByteArray &id)
{
  m_privateId = id;
  m_id = SimpleID::make(id, SimpleID::ServerId);
}

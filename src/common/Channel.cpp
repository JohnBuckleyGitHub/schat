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

#include "debugstream.h"

#include "Channel.h"
#include "net/SimpleID.h"
#include "text/HtmlFilter.h"


/*!
 * Добавление идентификатора в список каналов.
 */
bool Channels::add(const QByteArray &id)
{
  if (!Channel::isCompatibleId(id))
    return false;

  if (m_channels.contains(id))
    return false;

  m_channels.append(id);
  return true;
}


Channel::Channel()
  : m_synced(false)
  , m_valid(true)
{
}


Channel::Channel(const QByteArray &id, const QString &name)
  : m_synced(false)
  , m_valid(true)
{
  setId(id);
  setName(name);
}


Channel::~Channel()
{
}


bool Channel::setId(const QByteArray &id)
{
  m_id = id;
  m_feeds.setId(id);
  return validate(id.size() == SimpleID::DefaultSize);
}


bool Channel::setName(const QString &name)
{
  if (name.isEmpty())
    return validate(false);

  if (name.size() > (MaxNameLength * 2))
    return validate(false);

  m_name = name.simplified().left(MaxNameLength);
  if (name.size() < MinNameLengh)
    return validate(false);

  return validate(true);
}


/*!
 * Проверка идентификатора на принадлежность к допустимому типу.
 *
 * \param id Проверяемый идентификатор.
 * \return Тип идентификатора, если он допустимый или 0.
 */
int Channel::isCompatibleId(const QByteArray &id)
{
  int type = SimpleID::typeOf(id);
  if (type != SimpleID::ChannelId && type != SimpleID::UserId && type != SimpleID::ServerId)
    return type;

  return 0;
}

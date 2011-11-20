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

#include "Account.h"
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
  : m_account(0)
  , m_synced(false)
  , m_type(SimpleID::InvalidId)
{
}


Channel::Channel(const QByteArray &id, const QString &name)
  : m_account(0)
  , m_synced(false)
  , m_type(SimpleID::InvalidId)
{
  setId(id);
  setName(name);
}


Channel::~Channel()
{
  if (m_account)
    delete m_account;
}


bool Channel::isValid() const
{
  if (m_name.isEmpty())
    return false;

  if (m_id.isEmpty())
    return false;

  return true;
}


/*!
 * Установка идентификатора канала.
 */
bool Channel::setId(const QByteArray &id)
{
  int type = Channel::isCompatibleId(id);
  if (type == 0)
    return false;

  m_id = id;
  m_type = type;
  m_feeds.setChannel(this);
  return true;
}


bool Channel::setName(const QString &name)
{
  if (name.isEmpty())
    return false;

  QString tmp = name.simplified().left(MaxNameLength);
  if (tmp.size() < MinNameLengh)
    return false;

  m_name = tmp;
  return true;
}


void Channel::setAccount(Account *account)
{
  if (!m_account)
    m_account = new Account();

  *m_account = *account;
}


/*!
 * Проверка идентификатора на принадлежность к допустимому типу.
 *
 * \param id Проверяемый идентификатор.
 * \return Тип идентификатора или 0 если идентификатор не совместим или не корректен.
 */
int Channel::isCompatibleId(const QByteArray &id)
{
  int type = SimpleID::typeOf(id);
  if (type == SimpleID::ChannelId || type == SimpleID::UserId || type == SimpleID::ServerId)
    return type;

  return 0;
}

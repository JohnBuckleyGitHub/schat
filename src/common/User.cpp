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

#include <QDir>

#include "net/Protocol.h"
#include "User.h"

User::User()
  : m_valid(true)
{
}


User::User(const QString &nick)
  : m_valid(true)
{
  setNick(nick);
}


User::User(const User *other)
  : m_valid(other->isValid())
  , m_id(other->id())
  , m_nick(other->nick())
{
}


bool User::addChannel(const QByteArray &id)
{
  if (id.size() != Protocol::IdSize)
    return false;

  if (m_channels.contains(id))
    return false;

  m_channels.append(id);
  return true;
}


bool User::removeChannel(const QByteArray &id)
{
  if (id.size() != Protocol::IdSize)
    return false;

  if (!m_channels.contains(id))
    return false;

  m_channels.removeAll(id);
  return true;
}


bool User::setId(const QByteArray &id)
{
  m_id = id;
  return validate(id.size() == Protocol::IdSize);
}


bool User::setNick(const QString &nick)
{
  if (nick.size() > (MaxNickLength * 2))
    return validate(false);

  m_nick = nick.simplified().left(MaxNickLength);
  if (nick.size() < MinNickLengh)
    return validate(false);

  return validate(true);
}


/*!
 * \todo http://www.prog.org.ru/topic_7694_0.html;topicseen
 */
QString User::defaultNick()
{
  return QDir::home().dirName();
}

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

#include "debugstream.h"
#include "net/SimpleID.h"
#include "User.h"

User::User()
  : m_valid(true)
  , m_gender(Male)
  , m_status(OfflineStatus)
{
}


User::User(const QString &nick)
  : m_valid(true)
  , m_gender(Male)
  , m_status(OfflineStatus)
{
  setNick(nick);
}


User::User(const User *other)
  : m_valid(other->isValid())
  , m_gender(other->rawGender())
  , m_status(other->status())
  , m_id(other->id())
  , m_host(other->host())
  , m_nick(other->nick())
  , m_userAgent(other->userAgent())
{
}


User::~User()
{
  SCHAT_DEBUG_STREAM("~" << this << m_nick)
}


bool User::setId(const QByteArray &id)
{
  m_id = id;
  return validate(id.size() == SimpleID::DefaultSize);
}


bool User::setNick(const QString &nick)
{
  if (nick.size() > (MaxNickLength * 2))
    return validate(false);

  QString tmp = nick.simplified().left(MaxNickLength);
  if (tmp.size() < MinNickLengh)
    return validate(false);

  m_nick = tmp;
  return validate(true);
}


bool User::isValidNick(const QString &nick)
{
  if (nick.size() > (MaxNickLength * 2))
    return false;

  if (nick.simplified().left(MaxNickLength).size() < MinNickLengh)
    return false;

  return true;
}


/*!
 * \todo ! http://www.prog.org.ru/topic_7694_0.html;topicseen
 */
QString User::defaultNick()
{
  return QDir::home().dirName();
}


int User::color() const
{
  if (m_gender >= Unknown)
    return Default;

  int out = m_gender;
  if (gender() == Female)
    out -= Female;

  if (out > Thief)
    return Default;

  return out;
}


int User::gender() const
{
  if (m_gender == Ghost)
    return Ghost;

  if (m_gender == Unknown || m_gender > Ghost)
    return Unknown;

  if (m_gender < Female)
    return Male;

  return Female;
}


void User::setColor(Color color)
{
  if (m_gender >= Unknown)
    return;

  m_gender = gender() + color;
}


void User::setColor(int color)
{
  if (color < 0 || color > Thief)
    color = Default;

  setColor(static_cast<Color>(color));
}


/*!
 * Установка пола.
 */
void User::setGender(Gender gender)
{
  m_gender = gender + color();
}


/*!
 * Добавление идентификатора в список идентификаторов.
 *
 * \param type Имя списка, в котором будет сохранён идентификатор.
 * \param id   Идентификатор.
 * \return true в случае успеха.
 */
bool User::addId(int type, const QByteArray &id)
{
  if (id.size() != SimpleID::DefaultSize)
    return false;

  if (type < SimpleID::MinUserRoleId || type > SimpleID::MaxUserRoleId)
    return false;

  if (m_ids.contains(type) && m_ids.value(type).contains(id))
    return false;

  m_ids[type].append(id);
  return true;
}


bool User::containsId(int type, const QByteArray &id)
{
  if (id.size() != SimpleID::DefaultSize)
    return false;

  if (!m_ids.contains(type))
    return false;

  return m_ids.value(type).contains(id);
}


/*!
 * Удаление списка идентификаторов с именем \p type.
 */
bool User::remove(int type)
{
  if (m_ids.contains(type))
    m_ids.remove(type);

  return true;
}


/*!
 * Удаление идентификатора из списка идентификаторов.
 *
 * \param type Имя списка, из которого будет удалён идентификатор.
 * \param id   Идентификатор.
 * \return true в случае успешного удаления или если идентификатор отсутствовал в списке.
 */
bool User::removeId(int type, const QByteArray &id)
{
  if (id.size() != SimpleID::DefaultSize)
    return false;

  if (m_ids.contains(type) && m_ids.value(type).contains(id)) {
    m_ids[type].removeAll(id);
    if (m_ids.value(type).isEmpty())
      m_ids.remove(type);
    return true;
  }

  return true;
}


int User::count(int type)
{
  if (!m_ids.contains(type))
    return 0;

  return m_ids.value(type).size();
}


QList<QByteArray> User::ids(int type)
{
  if (!m_ids.contains(type))
    return QList<QByteArray>();

  return m_ids.value(type);
}


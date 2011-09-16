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
  , m_serverNumber(0)
{
  m_channels.append(m_id);
  m_groups.append(QLatin1String("regular"));
}


User::User(const QString &nick)
  : m_valid(true)
  , m_gender(Male)
  , m_status(OfflineStatus)
  , m_serverNumber(0)
{
  setNick(nick);
  m_channels.append(m_id);
  m_groups.append(QLatin1String("regular"));
}


User::User(const User *other)
  : m_valid(other->isValid())
  , m_gender(other->rawGender())
  , m_status(other->status())
  , m_id(other->id())
  , m_channels(other->channels())
  , m_host(other->host())
  , m_nick(other->nick())
  , m_userAgent(other->userAgent())
  , m_serverNumber(other->serverNumber())
{
  m_groups.append(QLatin1String("regular"));
}


User::~User()
{
  SCHAT_DEBUG_STREAM("~" << this << m_nick)
}


void User::clear()
{
  m_channels.clear();
  m_channels.append(SimpleID::setType(SimpleID::ChannelId, m_id));
}


bool User::setId(const QByteArray &id)
{
  m_id = id;
  m_channels[0] = SimpleID::setType(SimpleID::ChannelId, m_id);
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
  if (m_gender >= Unknown && m_gender <= Bot)
    return m_gender;

  if (m_gender > Bot)
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
  if (gender >= Unknown)
    m_gender = gender;
  else
    m_gender = gender + color();
}


bool User::addChannel(const QByteArray &id)
{
  if (id.size() != SimpleID::DefaultSize)
    return false;

  if (SimpleID::typeOf(id) != SimpleID::ChannelId)
    return false;

  if (m_channels.contains(id))
    return true;

  m_channels.append(id);
  return true;
}


bool User::removeChannel(const QByteArray &id)
{
  if (m_channels.size() == 1)
    return false;

  if (!m_channels.contains(id))
    return false;

  if (channel() == id)
    return false;

  m_channels.removeAll(id);
  return true;
}


/*!
 * Установка статуса пользователя из строки.
 * Формат строки: число + разделитель ; + опциональный текст статуса.
 * В тексте статуса не может содержаться символ ';', он должен быть заменён на %3B.
 *
 * \sa statusToString()
 *
 * \return true в случае если строка корректна и статус установлен.
 */
bool User::setStatus(const QString &text)
{
  if (text.isEmpty())
    return false;

  int index = text.indexOf(QLatin1Char(';'));

  if (index == -1)
    return false;

  bool ok;
  int status = text.left(index).toUInt(&ok);
  if (!ok)
    return false;

  index = text.lastIndexOf(QLatin1Char(';'));

  if (text.size() - index == 1)
    setStatus(status);
  else
    setStatus(status, text.mid(index + 1).replace(QLatin1String("%3B"), QLatin1String(";")));

  return true;
}


QString User::statusText(int status) const
{
  if (status < 0 || status > 255)
    return m_statuses.value(m_status);

  return m_statuses.value(status);
}


/*!
 * Формирует строку статуса.
 */
QString User::statusToString(int status, const QString &text)
{
  QString t = text;
  t.replace(";", "%3B");

  return QString::number(status) + ";" + t;
}


void User::setStatus(int status, const QString &text)
{
  setStatus(status);
  m_statuses[status] = text;
}

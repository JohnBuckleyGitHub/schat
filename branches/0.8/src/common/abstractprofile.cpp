/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2013 IMPOMEZIA <schat@impomezia.com>
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

#include "abstractprofile.h"
#include "version.h"

/*!
 * Конструктор копирования.
 */
AbstractProfile::AbstractProfile(const AbstractProfile &other)
  : QObject(),
  m_male(other.m_male),
  m_byeMsg(other.m_byeMsg),
  m_fullName(other.m_fullName),
  m_host(other.m_host),
  m_nick(other.m_nick),
  m_userAgent(other.m_userAgent),
  m_status(other.m_status)
{
}


/*!
 * Конструктор класса AbstractProfile.
 */
AbstractProfile::AbstractProfile(const QStringList &list, QObject *parent)
  : QObject(parent)
{
  unpack(list);
  m_status = 0;
}


/*!
 * Конструктор класса AbstractProfile.
 */
AbstractProfile::AbstractProfile(QObject *parent)
  : QObject(parent)
{
  m_male = true;
  m_nick = defaultNick();
#ifdef SCHAT_CLIENT
  m_userAgent = QString("IMPOMEZIA Simple Chat/%1").arg(SCHAT_VERSION);
#else
  m_userAgent = QString("IMPOMEZIA Simple Chat Daemon/%1").arg(SCHAT_VERSION);
#endif
  if (QSysInfo::WordSize == 64)
    m_userAgent += " (64 bit)";

  m_status = 0;
}


AbstractProfile& AbstractProfile::operator=(const AbstractProfile &other)
{
  m_male      = other.m_male;
  m_byeMsg    = other.m_byeMsg;
  m_fullName  = other.m_fullName;
  m_host      = other.m_host;
  m_nick      = other.m_nick;
  m_userAgent = other.m_userAgent;
  m_status    = other.m_status;
  return *this;
}


bool AbstractProfile::isValidNick(const QString &n)
{
  QString nick = n.simplified();

  if (nick.size() > MaxNickLength)
    return false;
  if (nick.isEmpty())
    return false;
  if (nick.startsWith(QChar('#')))
    return false;
  if (nick == ".")
    return false;

  return true;
}


bool AbstractProfile::isValidUserAgent(const QString &a)
{
  if (a.isEmpty())
    return false;

  QStringList list = a.split(QChar('/'));
  if (list.size() == 2)
    return true;
  else
    return false;
}


QStringList AbstractProfile::pack() const
{
  QStringList list;
  list << m_nick << m_fullName << m_byeMsg << m_userAgent << m_host << gender();
  return list;
}


/*!
 * Автоматическое определение ника по умолчанию.
 */
QString AbstractProfile::defaultNick()
{
  #ifdef Q_OS_WINCE
  QSettings s("HKEY_CURRENT_USER\\ControlPanel\\Owner", QSettings::NativeFormat);
  QString name = s.value("Name").toString();
  if (name.isEmpty())
    name = "WindowsMobile";
  return name;
  #else
  return QDir::home().dirName();
  #endif
}


void AbstractProfile::setStatus(quint32 status)
{
  m_status = status;
  emit statusChanged(status);
}


void AbstractProfile::unpack(const QStringList &list)
{
  setNick(list.at(Nick));
  setFullName(list.at(FullName));
  setByeMsg(list.at(ByeMsg));
  setUserAgent(list.at(UserAgent));
  setHost(list.at(Host));
  setGender(list.at(Gender));
}

/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2009 IMPOMEZIA <schat@impomezia.com>
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

#include <QtCore>

#include "abstractprofile.h"
#include "version.h"

/*!
 * \brief Конструктор класса AbstractProfile.
 */
AbstractProfile::AbstractProfile(QObject *parent)
  : QObject(parent)
{
  m_male = true;
  m_nick = QDir::home().dirName();
#ifdef SCHAT_CLIENT
  m_userAgent = QString("IMPOMEZIA Simple Chat/%1").arg(SCHAT_VERSION);
#else
  m_userAgent = QString("IMPOMEZIA Simple Chat Daemon/%1").arg(SCHAT_VERSION);
#endif
  m_status = 0;
}


/** [public]
 *
 */
AbstractProfile::AbstractProfile(const QStringList &list, QObject *parent)
  : QObject(parent)
{
  unpack(list);
  m_status = 0;
}


/** [public]
 *
 */
AbstractProfile::~AbstractProfile()
{
#ifdef SCHAT_DEBUG
  qDebug() << "AbstractProfile::~AbstractProfile()";
#endif
}


/** [public] static
 *
 */
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


/** [public] static
 *
 */
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


/** [public]
 *
 */
QStringList AbstractProfile::pack() const
{
  QStringList list;
  list << m_nick << m_fullName << m_byeMsg << m_userAgent << m_host << gender();
  return list;
}


/** [public]
 *
 */
void AbstractProfile::unpack(const QStringList &list)
{
  setNick(list.at(Nick));
  setFullName(list.at(FullName));
  setByeMsg(list.at(ByeMsg));
  setUserAgent(list.at(UserAgent));
  setHost(list.at(Host));
  setGender(list.at(Gender));
}

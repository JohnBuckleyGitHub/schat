/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QtCore>

#include "profile.h"
#include "version.h"


/** [public]
 * 
 */
Profile::Profile(const QString &nick, const QString &fullName, quint8 sex, QObject *parent)
  : QObject(parent)
{
  m_nick = nick.simplified().left(MaxNickLength);
  m_fullName = fullName.simplified().left(MaxNameLength);
  m_sex = sex;
  m_userAgent = QString("Simple Chat/%1").arg(SCHAT_VERSION);
}


/** [public]
 * 
 */
Profile::Profile(QObject *parent)
  : QObject(parent)
{
  m_sex = 0;
  m_nick = QDir::home().dirName();
  m_userAgent = QString("Simple Chat/%1").arg(SCHAT_VERSION);
}


/** [public]
 * 
 */
Profile::Profile(quint8 sex, const QStringList &list, QObject *parent)
  : QObject(parent)
{
  m_sex = sex;
  fromList(list);
}


/** [public]
 * 
 */
Profile::~Profile()
{
  qDebug() << "Profile::~Profile()";
}


/** [public]
 * 
 */
bool Profile::fromList(const QStringList &list)
{
  if (list.size() != 4)
    return false;
  
  m_nick      = list.at(0).simplified().left(MaxNickLength);
  m_fullName  = list.at(1).simplified().left(MaxNameLength);
  m_userAgent = list.at(2);
  m_host      = list.at(3);
  return true;
}


/** [public]
 * 
 */
bool Profile::isValidUserAgent() const
{
  if (m_userAgent.isEmpty())
    return false;
  
  QStringList list = m_userAgent.split(QChar('/'));
  if (list.size() == 2)
    return true;
  else
    return false;
}


/** [public]
 * 
 */
QString Profile::toolTip()
{
  QString a = m_userAgent;
  QString n;
  m_fullName.isEmpty() ? n = tr("&lt;не указано&gt;") : n = m_fullName;
  a.replace(QChar('/'), QChar(' '));
  
  return tr("<h3><img src='%1' align='left'> %2</h3>"
            "<table><tr><td>Настоящее имя:</td><td>%3</td></tr>"
            "<tr><td>Клиент:</td><td>%4</td></tr>"
            "<tr><td>IP-адрес:</td><td>%5</td></tr></table>")
            .arg(sexIconString(m_sex)).arg(m_nick).arg(n).arg(a).arg(m_host); 
}

/** [public]
 * 
 */
QStringList Profile::toList() const
{
  return QStringList() << m_nick << m_fullName << m_userAgent << m_host;
}


/** [public]
 * 
 */
bool Profile::isValidNick(const QString &n)
{
  QString nick = n.simplified();
  
  if (nick.isEmpty())
    return false;
  if (nick.startsWith(QChar('#')))
    return false;
  else if (nick == ".")
    return false;
  
  return true;
}


/** [public]
 * 
 */
QString Profile::sexIconString(quint8 sex)
{
  if (sex)
    return ":/images/female.png";
  else
    return ":/images/male.png"; 
}


/** [public]
 * 
 */
void Profile::toStream(QDataStream &stream) const
{
  stream << m_sex << m_nick << m_fullName << m_userAgent;
}

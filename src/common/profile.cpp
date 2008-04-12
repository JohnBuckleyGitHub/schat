/* $Id$
 * Simple Chat
 * Copyright © 2008 IMPOMEZIA (http://impomezia.net.ru)
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
  Nick = nick;
  FullName = fullName;
  Sex = sex;
  UserAgent = QString("Simple Chat/%1").arg(SCHAT_VERSION);
}


/** [public]
 * 
 */
Profile::Profile(QObject *parent)
  : QObject(parent)
{
  Sex = 0;
  Nick = QDir::home().dirName();
  UserAgent = QString("Simple Chat/%1").arg(SCHAT_VERSION);
}


/** [public]
 * 
 */
Profile::Profile(quint8 sex, const QStringList &list, QObject *parent)
  : QObject(parent)
{
  Sex = sex;
  fromList(list);
}


/** [public]
 * 
 */
bool Profile::fromList(const QStringList &list)
{
  if (list.size() != 4)
    return false;
  
  Nick      = list.at(0);
  FullName  = list.at(1);
  UserAgent = list.at(2);
  Host      = list.at(3);
  return true;
}


/** [public]
 * 
 */
bool Profile::isValidNick() const
{
  if (Nick.isEmpty() || Nick == "#DUBLICATE" || Nick == "#main")
    return false;
  else
    return true;
}


/** [public]
 * 
 */
bool Profile::isValidUserAgent() const
{
  if (UserAgent.isEmpty())
    return false;
  
  QStringList list = UserAgent.split('/');
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
  QString a = UserAgent;
  QString n;
  FullName.isEmpty() ? n = tr("&lt;не указано&gt;") : n = FullName;
  a.replace('/', ' ');
  
  return tr("<h3><img src='%1' align='left'> %2</h3>"
            "<table><tr><td>Настоящее имя:</td><td>%3</td></tr>"
            "<tr><td>Клиент:</td><td>%4</td></tr>"
            "<tr><td>IP-адрес:</td><td>%5</td></tr></table>")
            .arg(sexIconString(Sex)).arg(Nick).arg(n).arg(a).arg(Host); 
}

/** [public]
 * 
 */
QStringList Profile::toList() const
{
  return QStringList() << Nick << FullName << UserAgent << Host;
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
  stream << Sex << Nick << FullName << UserAgent;
}

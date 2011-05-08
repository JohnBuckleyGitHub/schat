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

#include <QByteArray>
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QUuid>

#define SCHAT_RANDOM_CLIENT_ID

#include "net/Protocol.h"
#include "net/SimpleID.h"

/*!
 * Возвращает тип идентификатора.
 */
int SimpleID::typeOf(const QByteArray &id)
{
  if (id.size() != DefaultSize)
    return InvalidId;

  return id.at(DefaultSize - 1);
}


/*!
 * Получение идентификатора приватного канала, методом получения хеша из
 * сортированных идентификаторов пользователей.
 */
QByteArray SimpleID::id(const QByteArray &userId1, const QByteArray &userId2)
{
  QList<QByteArray> tmp;
  tmp.append(userId1);
  tmp.append(userId2);
  qSort(tmp);

  return QCryptographicHash::hash(tmp.at(0) + tmp.at(1), QCryptographicHash::Sha1) += PrivateChannelId;
}


QByteArray SimpleID::session(const QByteArray &id)
{
  return QCryptographicHash::hash(QString(id + QUuid::createUuid()).toLatin1(), QCryptographicHash::Sha1) += SessionId;
}


/*!
 * Получение уникального идентификатора клиента на основе
 * mac адреса первого активного сетевого интерфейса.
 */
QByteArray SimpleID::uniqueId()
{
  #if !defined(SCHAT_DAEMON) && defined(SCHAT_RANDOM_CLIENT_ID)
  return QCryptographicHash::hash(QUuid::createUuid().toString().toLatin1(), QCryptographicHash::Sha1) += UniqueUserId;
  #endif

  QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface iface, all) {
    QString hw = iface.hardwareAddress();
    if (!hw.isEmpty() && iface.flags().testFlag(QNetworkInterface::IsUp) && iface.flags().testFlag(QNetworkInterface::IsRunning)) {
      return QCryptographicHash::hash(hw.toLatin1(), QCryptographicHash::Sha1) += UniqueUserId;
    }
  }

  return QCryptographicHash::hash("", QCryptographicHash::Sha1);
}

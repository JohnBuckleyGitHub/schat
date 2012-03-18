/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2012 IMPOMEZIA <schat@impomezia.com>
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
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QNetworkInterface>
#include <QSettings>
#include <QSysInfo>
#include <QUuid>

#if defined(SCHAT_WEBKIT)
#include <qwebkitversion.h>
#endif

//#define SCHAT_RANDOM_CLIENT_ID

#include "base32/base32.h"
#include "net/Protocol.h"
#include "net/SimpleID.h"

/*!
 * Возвращает тип идентификатора.
 */
int SimpleID::typeOf(const QByteArray &id)
{
  if (id.size() != DefaultSize)
    return InvalidId;

  return quint8(id.at(DefaultSize - 1));
}


/*!
 * Декодирование идентификатора из Base32.
 *
 * \param id Идентификатор, размер должен быть равен EncodedSize.
 * \return Кодированный идентификатор или пустой массив в случае ошибки.
 */
QByteArray SimpleID::decode(const QByteArray &id)
{
  if (id.size() != EncodedSize)
    return QByteArray();

  char outbuf[DefaultSize + 1];
  if (base32_decode(reinterpret_cast<const uchar *>(id.constData()), reinterpret_cast<uchar *>(outbuf)) != DefaultSize)
    return QByteArray();

  return QByteArray(outbuf, DefaultSize);
}


QByteArray SimpleID::decode(const QString &id)
{
  return decode(id.toLatin1());
}


/*!
 * Кодирование идентификатора в Base32.
 *
 * \param id Идентификатор, размер должен быть равен DefaultSize.
 * \return Кодированный идентификатор или пустой массив в случае ошибки.
 */
QByteArray SimpleID::encode(const QByteArray &id)
{
  if (id.size() != DefaultSize)
    return QByteArray();

  char outbuf[41];
  base32_encode(reinterpret_cast<const uchar *>(id.constData()), DefaultSize, reinterpret_cast<uchar *>(outbuf));

  return QByteArray(outbuf, EncodedSize);
}


QByteArray SimpleID::fromBase32(const QByteArray &base32)
{
  int size = UNBASE32_LEN(base32.size()) + 9;
  char *outbuf = new char[size];

  int len = base32_decode(reinterpret_cast<const uchar *>(base32.constData()), reinterpret_cast<uchar *>(outbuf));

  QByteArray out = QByteArray(outbuf, len);
  delete [] outbuf;
  return out;
}


/*!
 * Создание идентификатора.
 *
 * \param data Данные на основе которых будет создан идентификатор.
 * \param type Тип идентификатора.
 */
QByteArray SimpleID::make(const QByteArray &data, Types type)
{
  return QCryptographicHash::hash(data, QCryptographicHash::Sha1) += type;
}


QByteArray SimpleID::password(const QString &password, const QByteArray &salt)
{
  return make(password.toUtf8() + salt, PasswordId);
}


QByteArray SimpleID::randomId(Types type, const QByteArray &salt)
{
  return make(salt + QUuid::createUuid().toString().toLatin1(), type);
}


QByteArray SimpleID::setType(int type, const QByteArray &id)
{
  if (id.size() != DefaultSize)
    return id;

  return id.left(DefaultSize - 1) += type;
}


QByteArray SimpleID::toBase32(const QByteArray &data)
{
  int size = BASE32_LEN(data.size()) + 1;
  char *outbuf = new char[size];

  base32_encode(reinterpret_cast<const uchar *>(data.constData()), data.size(), reinterpret_cast<uchar *>(outbuf));

  QByteArray out = QByteArray(outbuf, size - 1);
  out.replace('=', "");
  delete [] outbuf;
  return out;
}


/*!
 * Получение уникального идентификатора клиента на основе
 * mac адреса первого активного сетевого интерфейса.
 */
QByteArray SimpleID::uniqueId()
{
  #if defined(SCHAT_RANDOM_CLIENT_ID)
  return QCryptographicHash::hash(QUuid::createUuid().toString().toLatin1(), QCryptographicHash::Sha1) += UniqueUserId;
  #endif

  QList<QNetworkInterface> all = QNetworkInterface::allInterfaces();
  foreach (QNetworkInterface iface, all) {
    QString hw = iface.hardwareAddress();
    if (!hw.isEmpty() && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) && iface.flags().testFlag(QNetworkInterface::IsUp) && iface.flags().testFlag(QNetworkInterface::IsRunning)) {
      return make(hw.toLatin1(), UniqueUserId);
    }
  }

  return make("", UniqueUserId);
}

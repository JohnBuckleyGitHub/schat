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
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QNetworkInterface>
#include <QSysInfo>
#include <QUuid>

#if defined(SCHAT_WEBKIT)
#include <qwebkitversion.h>
#endif

#define SCHAT_RANDOM_CLIENT_ID

#include "net/Protocol.h"
#include "net/SimpleID.h"

QString SimpleID::m_userAgent;

/*!
 * Проверяет идентификатор на вхождение в диапазон пользовательских идентификаторов.
 *
 * \param id Проверяемый идентификатор.
 * \return true если идентификатор входит в диапазон.
 */
bool SimpleID::isUserRoleId(const QByteArray &id)
{
  int type = typeOf(id);
  if (type < MinUserRoleId || type > MaxUserRoleId)
    return false;

  return true;
}


/*!
 * Проверяет идентификатор на вхождение в диапазон пользовательских идентификаторов,
 * а также на принадлежность к пользователю \p userId.
 *
 * \param userId Идентификатор пользователя.
 * \param id     Проверяемый идентификатор.
 * \return true если идентификатор входит в диапазон.
 */
bool SimpleID::isUserRoleId(const QByteArray &userId, const QByteArray &id)
{
  if (!isUserRoleId(id))
    return false;

  if (userId.left(DefaultSize - 1) == id.left(DefaultSize - 1))
    return true;

  return false;
}


/*!
 * Возвращает тип идентификатора.
 */
int SimpleID::typeOf(const QByteArray &id)
{
  if (id.size() != DefaultSize)
    return InvalidId;

  return quint8(id.at(DefaultSize - 1));
}


QByteArray SimpleID::fromBase64(const QByteArray &base64)
{
  QByteArray tmp = base64;
  tmp.replace('-', '+');
  tmp.replace('_', '/');
  return QByteArray::fromBase64(tmp);
}


QByteArray SimpleID::session(const QByteArray &id)
{
  return QCryptographicHash::hash(QString(id + QUuid::createUuid()).toLatin1(), QCryptographicHash::Sha1) += SessionId;
}


QByteArray SimpleID::setType(int type, const QByteArray &id)
{
  if (id.size() != DefaultSize)
    return id;

  return id.left(DefaultSize - 1) += type;
}


/*!
 * Выполняет преобразование в Base64 с заменой недопустимых для файловой системы символов,
 * в соответствии с RFC3548 http://tools.ietf.org/html/rfc3548#page-6.
 */
QByteArray SimpleID::toBase64(const QByteArray &id)
{
  QByteArray out = id.toBase64();
  out.replace('+', '-');
  out.replace('/', '_');
  return out;
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
    if (!hw.isEmpty() && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) && iface.flags().testFlag(QNetworkInterface::IsUp) && iface.flags().testFlag(QNetworkInterface::IsRunning)) {
      return QCryptographicHash::hash(hw.toLatin1(), QCryptographicHash::Sha1) += UniqueUserId;
    }
  }

  return QCryptographicHash::hash("", QCryptographicHash::Sha1) += UniqueUserId;
}


/*!
 * Строка UserAgent по умолчанию.
 */
QString SimpleID::userAgent()
{
  if (!m_userAgent.isEmpty())
    return m_userAgent;

  QString out = QLatin1String("SimpleChat/") + QCoreApplication::applicationVersion();

  out += QLatin1String(" (");
  #if defined(Q_OS_FREEBSD)
  out += QLatin1String("FreeBSD");
  #elif defined(Q_OS_LINUX)
  out += QLatin1String("Linux");

  #elif defined(Q_OS_MAC)
  out += QLatin1String("Mac OS X");
  QSysInfo::MacVersion version = QSysInfo::MacintoshVersion;
  if (version == QSysInfo::MV_10_5)
    out += QLatin1String(" 10.5");
  else if (version == QSysInfo::MV_10_6)
    out += QLatin1String(" 10.6");

  #elif defined(Q_OS_NETBSD)
  out += QLatin1String("NetBSD");
  #elif defined(Q_OS_OPENBSD)
  out += QLatin1String("OpenBSD");

  #elif defined(Q_OS_WIN32)
  out += QLatin1String("Windows");
  QSysInfo::WinVersion version = QSysInfo::WindowsVersion;
  if (version & QSysInfo::WV_NT_based) {
    out += QLatin1String(" NT");
    if (version == QSysInfo::WV_5_0)
      out += QLatin1String(" 5.0");
    else if (version == QSysInfo::WV_5_1)
      out += QLatin1String(" 5.1");
    else if (version == QSysInfo::WV_5_2)
      out += QLatin1String(" 5.2");
    else if (version == QSysInfo::WV_6_0)
      out += QLatin1String(" 6.0");
    else if (version == QSysInfo::WV_6_1)
      out += QLatin1String(" 6.1");
  }
  #else
  out += QLatin1String("Unknown");
  #endif

  out += QLatin1String("; Qt ");
  out += qVersion();

  #if defined(SCHAT_WEBKIT)
  out += QLatin1String("; WebKit ");
  out += qWebKitVersion();
  #endif

  if (QSysInfo::WordSize == 64)
    out += QLatin1String("; x64");

  out += ")";

  return out;
}

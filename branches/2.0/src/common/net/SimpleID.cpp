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

QString SimpleID::m_userAgent;

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


/*!
 * Кодирование идентификатора в Base32.
 * Для результата используется символы в нижнем регистре, могут быть проблемы со сторонними реализациями Base32.
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


QByteArray SimpleID::password(const QString &password, const QByteArray &salt)
{
  return QCryptographicHash::hash(password.toUtf8() + salt, QCryptographicHash::Sha1) += PasswordId;
}


QByteArray SimpleID::randomId(IdTypes type, const QByteArray &salt)
{
  return QCryptographicHash::hash(QString(salt + QUuid::createUuid()).toLatin1(), QCryptographicHash::Sha1) += type;
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
  out += linuxType();

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


#if defined(Q_OS_LINUX)
QString SimpleID::linuxType()
{
  QString out = QLatin1String("Linux");

  if (QFile::exists(QLatin1String("/etc/lsb-release"))) { // Ubuntu Linux
    QSettings s(QLatin1String("/etc/lsb-release"), QSettings::IniFormat);
    QString tmp = s.value(QLatin1String("DISTRIB_ID")).toString();
    if (tmp.isEmpty())
      return out;

    out = tmp;
    tmp = s.value(QLatin1String("DISTRIB_RELEASE")).toString();
    qDebug() << tmp;
    if (!tmp.isEmpty())
      out += QLatin1String(" ") + tmp;
  }
  else if (QFile::exists(QLatin1String("/etc/gentoo-release"))) {
    return QLatin1String("Gentoo Linux");
  }

  return out;
}
#endif

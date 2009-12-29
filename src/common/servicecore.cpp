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

#include "abstractprofile.h"
#include "protocol/CorePackets.h"
#include "schatmacro.h"
#include "servicecore.h"

/*!
 * \brief Конструктор класса ServiceCore.
 */
ServiceCore::ServiceCore(QObject *parent)
  : QObject(parent),
  m_profile(0),
  m_accepted(false),
  m_rx(0),
  m_tx(0),
  m_nextBlockSize(0)
{
  m_stream.setVersion(QDataStream::Qt_4_4);
}


/*!
 * Возвращает \a true если сервис находится в активном состоянии.
 */
bool ServiceCore::isReady() const
{
  if (m_socket && m_accepted && m_socket->state() == QTcpSocket::ConnectedState)
    return true;

  return false;
}


/*!
 * Базовая функция отправки пакета.
 */
bool ServiceCore::rawSend(const AbstractRawPacket &packet)
{
  SCHAT_DEBUG(this << "rawSend(" << packet.opcode() << ")")
  if (!isReady())
    return false;

  QByteArray size;
  QDataStream out(&size, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_4);
  QByteArray data = packet.data();
  out << quint16(data.size());
  m_tx += data.size() + 2;

  m_socket->write(size + data);
  return true;
}


/*!
 * \return Ник пользователя ассоциированного с сервисом.
 */
QString ServiceCore::nick() const
{
  if (m_profile)
    return m_profile->nick();

  return "";
}

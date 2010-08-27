/* $Id$
 * IMPOMEZIA Simple Chat
 * Copyright © 2008-2010 IMPOMEZIA <schat@impomezia.com>
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

#include "AbstractPeer.h"
#include "packet.h"
#include "schat.h"

/*!
 * Конструктор класса AbstractPeer.
 */
AbstractPeer::AbstractPeer(QObject *parent)
  : QObject(parent),
  m_rx(0),
  m_tx(0),
  m_nextBlockSize(0)
{
  SCHAT_DEBUG(this)
}


AbstractPeer::~AbstractPeer()
{
  SCHAT_DEBUG("~" << this)
}


/*!
 * Возвращает true если сервис находится в активном состоянии.
 */
bool AbstractPeer::isReady() const
{
  if (!m_socket)
    return false;

  if (m_socket->state() == QTcpSocket::ConnectedState)
    return true;

  return false;
}


/*!
 * Отправка пакета.
 */
bool AbstractPeer::send(const PacketBuilder &builder)
{
  if (!isReady())
    return false;

  m_socket->write(builder.data());
  m_tx += builder.size();

  return true;
}


/*!
 * Чтение пакета.
 * \param pcode Код пакета.
 * \param block Тело пакета.
 */
void AbstractPeer::readPacket(int pcode, const QByteArray &block)
{
  Q_UNUSED(pcode)
  Q_UNUSED(block)
  SCHAT_DEBUG(this << "readPacket(" << pcode << ", size:" << block.size() << "| rx:" << m_rx << "tx:" << m_tx)
}

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

#include "protocol/CorePackets.h"
#include "schatmacro.h"

/*!
 * Конструктор класса AbstractRawPacket.
 *
 * \param opcode Код пакета.
 */
AbstractRawPacket::AbstractRawPacket(quint16 opcode)
  : m_valid(true),
  m_opcode(opcode)
{
  SCHAT_DEBUG("AbstractRawPacket" << this << "opcode:" << opcode)
}


AbstractRawPacket::~AbstractRawPacket()
{
  SCHAT_DEBUG("~AbstractRawPacket()" << this);
}


/*!
 * Чтение сырого пакета.
 * Функция формирует поток данных, для дальнейшего разбора пакета.
 *
 * \param opcode Код пакета.
 * \param body   Тело пакета.
 */
bool AbstractRawPacket::read(quint16 opcode, const QByteArray &body)
{
  SCHAT_DEBUG(this << "read(" << opcode << ")");
  m_opcode = opcode;
  QDataStream stream(body);
  stream.setVersion(QDataStream::Qt_4_4);
  return readStream(&stream);;
}


QByteArray AbstractRawPacket::data() const
{
  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_4);
  out << m_opcode;
  writeStream(&out);
  return block;
}


/*!
 * Данная функция должна быть переопределена в наследниках,
 * для чтения специфичных данных пакета из потока.
 *
 * \param stream Указатель на поток данных.
 * \return \a false в случае ошибки чтения потока.
 */
bool AbstractRawPacket::readStream(QDataStream *stream)
{
  SCHAT_DEBUG(this << "readStream()")
  if (stream->status() == QDataStream::ReadCorruptData)
    return false;

  return true;
}


void AbstractRawPacket::writeStream(QDataStream *stream) const
{
  SCHAT_DEBUG(this << "writeStream()")
  Q_UNUSED(stream)
}

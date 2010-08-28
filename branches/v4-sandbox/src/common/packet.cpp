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

#include <QByteArray>
#include <QDataStream>
#include <QStringList>

#include "packet.h"
#include "schat.h"


/*!
 * Конструктор класса PacketBuilder.
 */
PacketBuilder::PacketBuilder(int pcode)
  : m_size(0)
{
  SCHAT_DEBUG("PacketBuilder()" << this)

  m_data = new QByteArray();
  m_stream = new QDataStream(m_data, QIODevice::WriteOnly);
  m_stream->setVersion(QDataStream::Qt_4_4);
  addPacket(pcode);
}


PacketBuilder::~PacketBuilder()
{
  SCHAT_DEBUG("~PacketBuilder()" << this)

  delete m_stream;
  delete m_data;
}


int PacketBuilder::size() const
{
  if (!m_size)
    return m_data->size();

  return m_size;
}


/*!
 * \return Сформированный пакет.
 */
QByteArray PacketBuilder::data() const
{
  SCHAT_DEBUG("PacketBuilder::data()" << this)
  Q_ASSERT(!m_pcodes.isEmpty());

  QByteArray header;
  QDataStream stream(&header, QIODevice::WriteOnly);
  stream.setVersion(QDataStream::Qt_4_6);
  stream << quint16(0); // размер данных пакета.
  if (m_pcodes.size() > 1) {
    /// \todo Добавить заголовок склеенного пакета.
  }

  /// \todo Добавить поддержку сжатия пакетов.
  m_size = m_data->size() + header.size();
  stream.device()->seek(0);
  stream << quint16(m_size - 2);

  SCHAT_DEBUG(" ^^^^ full size of data:" << m_size << "bytes")
  return header + *m_data;
}


void PacketBuilder::add(const QList<quint32> &data)
{
  *m_stream << data;
}


void PacketBuilder::add(const QStringList &data)
{
  *m_stream << data;
}


void PacketBuilder::add(Packet::DataTypes type, const QString &data)
{
  switch (type) {
    case Packet::UTF16:
      *m_stream << data;
      break;

    case Packet::UTF8:
      *m_stream << data.toUtf8();
      break;

    default:
      break;
  }
}


void PacketBuilder::add(Packet::DataTypes type, int data)
{
  switch (type) {
    case Packet::UINT16:
      *m_stream << quint16(data);
      break;

    case Packet::UINT8:
      *m_stream << quint8(data);
      break;

    default:
      break;
  }
}


/*!
 * Добавляет код нового пакета.
 */
void PacketBuilder::addPacket(int pcode)
{
  SCHAT_DEBUG("PacketBuilder::addPacket()" << this << pcode << ", size of data:" << m_data->size())

  m_pcodes.append(QPair<int, int>(pcode, m_data->size()));
  *m_stream << quint16(pcode);
}




/*!
 * Конструктор класса PacketReader.
 */
PacketReader::PacketReader(int pcode, const QByteArray &block)
  : m_count(1),
    m_pcode(pcode)

{
  SCHAT_DEBUG("PacketReader()" << this << pcode)

  m_stream = new QDataStream(block);
}


PacketReader::~PacketReader()
{
  SCHAT_DEBUG("~PacketReader()" << this)
}


/*!
 * Возвращает код следующего инкапсулированного пакета.
 * \todo Добавить поддержку склеенных пакетов.
 */
int PacketReader::getPacket()
{
  return m_pcode;
}


int PacketReader::getUint16() const
{
  quint16 out;
  *m_stream >> out;
  return out;
}


int PacketReader::getUint8() const
{
  quint8 out;
  *m_stream >> out;
  return out;
}


QList<quint32> PacketReader::getUint32List() const
{
  QList<quint32> out;
  *m_stream >> out;
  return out;
}


QList<quint8> PacketReader::getUint8List() const
{
  QList<quint8> out;
  *m_stream >> out;
  return out;
}


QString PacketReader::getUtf16() const
{
  QString out;
  *m_stream >> out;
  return out;
}


QStringList PacketReader::getUtf16List() const
{
  QStringList out;
  *m_stream >> out;
  return out;
}

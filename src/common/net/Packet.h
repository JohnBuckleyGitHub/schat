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

#ifndef PACKET_H_
#define PACKET_H_

#include <QByteArray>

class PacketReader;
class PacketWriter;

/*!
 * Базовый класс для данных виртуальных пакетов.
 */
class Packet
{
public:
  /// Опции для пакета Message.
  enum MessageOptions {
    GenericMessage = 0, ///< Стандартное сообщение, содержит одну текстовую стоку.
    ControlMessage = 1  ///< Содержит текстовую команду в виде независимой строки.
  };

  Packet(PacketReader *reader);
  virtual ~Packet() {}
  Packet(quint16 type);
  Packet(quint16 type, const QByteArray &sender, const QByteArray &dest);
  inline int subtype() const       { return m_subtype; }
  inline int type() const          { return m_type; }
  inline PacketReader *reader() const { return m_reader; }
  inline PacketWriter *writer() const { return m_writer; }
  inline QByteArray dest() const   { return m_dest; }
  inline QByteArray sender() const { return m_sender; }
  QByteArray data(QDataStream *stream);
  static int idType(const QByteArray &id);
  virtual bool isValid() const     { return true; }
  virtual void body() {}

protected:
  PacketReader *m_reader;
  PacketWriter *m_writer;
  QByteArray m_dest;
  QByteArray m_sender;
  quint16 m_type;
  quint8 m_subtype;
};

#endif /* PACKET_H_ */

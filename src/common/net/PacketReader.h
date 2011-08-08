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

#ifndef PACKETREADER_H_
#define PACKETREADER_H_

#include <QDataStream>

#include "net/SimpleID.h"

class SCHAT_EXPORT PacketReader
{
public:
  PacketReader(QDataStream *stream);
  inline int headerOption() const  { return m_headerOption; }
  inline int subtype() const       { return m_subtype; }
  inline int type() const          { return m_type; }
  inline QByteArray dest() const   { return m_dest; }
  inline QByteArray sender() const { return m_sender; }

  inline QString text() const {
    QByteArray data;
    *m_stream >> data;

    return QString(data);
  }

  template<class T>
  inline T get() const
  {
    T out;
    *m_stream >> out;
    return out;
  }

  inline QByteArray id() const {
    return m_device->read(SimpleID::DefaultSize);
  }

  inline QList<QByteArray> idList() const {
    quint32 size = get<quint32>();
    QList<QByteArray> out;
    if (size == 0)
      return out;

    for (unsigned int i = 0; i < size; ++i) {
      out.append(id());
    }

    return out;
  }

private:
  QByteArray m_dest;
  QByteArray m_sender;
  QDataStream *m_stream;
  QIODevice *m_device;
  quint16 m_type;
  quint8 m_headerOption;
  quint8 m_subtype;
};

#endif /* PACKETREADER_H_ */

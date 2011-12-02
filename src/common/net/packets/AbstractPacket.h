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

#ifndef ABSTRACTPACKET_H_
#define ABSTRACTPACKET_H_

#include <QVariant>

/*!
 * \deprecated Этот класс устаревший.
 */
class AbstractPacket
{
public:
  AbstractPacket(const QVariant &json = QVariant())
  : fields(0)
  , json(json)
  {}

  AbstractPacket(PacketReader *reader) { Q_UNUSED(reader) }
  virtual QByteArray data(QDataStream *stream) const = 0;

  mutable quint8 fields;   ///< Битовая маска дополнительный полей пакета.
  QVariant json;           ///< JSON данные.
};


#endif /* ABSTRACTPACKET_H_ */

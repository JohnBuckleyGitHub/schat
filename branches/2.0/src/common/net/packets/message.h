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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <QStringList>

#include "net/Packet.h"

class PacketReader;

/*!
 * - 01 byte  - MessageOptions.
 * - not fixed length (utf8) - Command (if set ControlMessage).
 * - not fixed length (utf8) - Message.
 */
class Message : public Packet
{
public:
  /// Типы сообщения.
  enum MessageOptions {
    GenericMessage = 0, ///< Стандартное сообщение, содержит одну текстовую стоку.
    ControlMessage = 1  ///< Содержит текстовую команду в виде независимой строки.
  };

  Message(const QString &command, const QString &message);
  Message(const QString &message);
  Message(PacketReader *reader);
  inline int options() const { return m_options; }
  inline QString command() const { return m_command; }
  inline QString message() const { return m_message; }
  void body();

protected:
  QString m_command;
  QString m_message;
  quint8 m_options;
};

#endif /* MESSAGE_H_ */

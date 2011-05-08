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

#include "net/PacketWriter.h"

class PacketReader;


/*!
 * Данные сообщения.
 */
class MessageData
{
public:
  /// Опции сообщения.
  enum Options {
    NoOptions = 0,      ///< Сообщение содержит только текст.
    ControlOption = 1,  ///< Сообщение содержит текстовую команду.
    NameOption = 2,     ///< Сообщение содержит имя.
    TextOption = 4      ///< Сообщение содержит текст сообщения.
  };

  MessageData()
  : options(NoOptions)
  , name(0)
  {}

  MessageData(const QByteArray &senderId, const QByteArray &destId, const QString &text)
  : options(TextOption)
  , destId(destId)
  , senderId(senderId)
  , text(text)
  , name(0)
  {}

  MessageData(const QByteArray &senderId, const QByteArray &destId, const QString &command, const QString &text)
  : options(ControlOption)
  , destId(destId)
  , senderId(senderId)
  , command(command)
  , text(text)
  , name(0)
  {
    if (!text.isEmpty()) {
      options |= TextOption;
    }
  }

  void autoSetOptions();

  int options;         ///< Опции сообщения.
  QByteArray destId;   ///< Идентификатор назначения.
  QByteArray senderId; ///< Идентификатор отправителя.
  QString command;     ///< Текстовая команда.
  QString text;        ///< Текст сообщения.
  quint64 name;        ///< Уникальное имя-счётчик сообещения.
};


/*!
 * Формирует пакет Protocol::MessagePacket.
 *
 * - 01 byte  - MessageData::Options.
 * - 08 bytes - Имя-счётчик (if set MessageData::NameOption).
 * - not fixed length (utf8) - Command (if set MessageData::ControlOption).
 * - not fixed length (utf8) - Message.
 */
class MessageWriter : public PacketWriter
{
public:
  MessageWriter(QDataStream *stream, const MessageData &data);
};


/*!
 * Читает пакет Protocol::MessagePacket.
 */
class MessageReader
{
public:
  MessageReader(PacketReader *reader);

  MessageData data;
};


class MessageUtils
{
public:
  MessageUtils() {}
  static QString toPlainText(const QString &text);
};

#endif /* MESSAGE_H_ */

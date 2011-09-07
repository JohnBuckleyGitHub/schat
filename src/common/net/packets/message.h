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
    NoOptions = 0,        ///< Сообщение содержит только текст.
    ControlOption = 1,    ///< Сообщение содержит текстовую команду.
    TimeOption = 2,       ///< Сообщение содержит отметку времени.
    TextOption = 4,       ///< Сообщение содержит текст сообщения.
    ExtraFlagsOption = 8,
    IdOption = 16
  };

  enum Flags {
    NoFlags = 0,
    OfflineFlag = 1
  };

  MessageData()
  : timestamp(0)
  , options(NoOptions)
  , flags(0)
  {}

  MessageData(const QByteArray &senderId, const QByteArray &destId, const QString &command, const QString &text)
  : senderId(senderId)
  , dest(QList<QByteArray>() << destId)
  , timestamp(0)
  , options(NoOptions)
  , flags(0)
  , command(command)
  , text(text)
  {
    autoSetOptions();
  }

  MessageData(const QByteArray &senderId, const QList<QByteArray> &dest, const QString &command, const QString &text)
  : senderId(senderId)
  , dest(dest)
  , timestamp(0)
  , options(NoOptions)
  , flags(0)
  , command(command)
  , text(text)
  {
    autoSetOptions();
  }

  void autoSetOptions()
  {
    options = NoOptions;

    if (timestamp > 0)
      options |= TimeOption;

    if (flags > 0)
      options |= ExtraFlagsOption;

    if (!id.isEmpty())
      options |= IdOption;

    if (!command.isEmpty())
      options |= ControlOption;

    if (!text.isEmpty())
      options |= TextOption;
  }

  QByteArray destId() const
  {
    if (dest.size())
      return dest.at(0);

    return QByteArray();
  }

  QByteArray senderId;    ///< Идентификатор отправителя.
  QList<QByteArray> dest; ///< Идентификаторы назначения.
  quint64 timestamp;       ///< Отметка времени.
  int options;            ///< Опции сообщения.
  quint16 flags;          ///< Дополнительные флаги сообщения.
  QByteArray id;          ///< Идентификатор сообщения.
  QString command;        ///< Текстовая команда.
  QString text;           ///< Текст сообщения.
};


/*!
 * Формирует пакет Protocol::MessagePacket.
 *
 * - 01 byte  - MessageData::Options.
 * - 08 bytes - Имя-счётчик (if set MessageData::NameOption).
 * - not fixed length (utf8) - Command (if set MessageData::ControlOption).
 * - not fixed length (utf8) - Message.
 */
class SCHAT_EXPORT MessageWriter : public PacketWriter
{
public:
  MessageWriter(QDataStream *stream, const MessageData &data, bool echo = false);
};


/*!
 * Читает пакет Protocol::MessagePacket.
 */
class SCHAT_EXPORT MessageReader
{
public:
  MessageReader(PacketReader *reader);

  MessageData data;
};


class SCHAT_EXPORT MessageUtils
{
public:
  MessageUtils() {}
  static bool remove(const QString &cmd, QString &msg);
  static QString htmlFilter(const QString &html, int left = 2048, bool strict = true);
  static QString toPlainText(const QString &text);
};

#endif /* MESSAGE_H_ */

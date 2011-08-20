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
  , timestamp(0)
  , name(0)
  {}

  MessageData(const QByteArray &senderId, const QByteArray &destId, const QString &command, const QString &text)
  : options(NoOptions)
  , senderId(senderId)
  , timestamp(0)
  , dest(QList<QByteArray>() << destId)
  , command(command)
  , text(text)
  , name(0)
  {
    autoSetOptions();
  }

  MessageData(const QByteArray &senderId, const QList<QByteArray> &dest, const QString &command, const QString &text)
  : options(NoOptions)
  , senderId(senderId)
  , timestamp(0)
  , dest(dest)
  , command(command)
  , text(text)
  , name(0)
  {
    autoSetOptions();
  }

  void autoSetOptions()
  {
    options = NoOptions;

    if (!command.isEmpty())
      options |= ControlOption;

    if (name > 0)
      options |= NameOption;

    if (!text.isEmpty())
      options |= TextOption;
  }

  QByteArray destId() const
  {
    if (dest.size())
      return dest.at(0);

    return QByteArray();
  }

  int options;            ///< Опции сообщения.
  QByteArray senderId;    ///< Идентификатор отправителя.
  qint64 timestamp;       ///< Отметка времени.
  QList<QByteArray> dest; ///< Идентификаторы назначения.
  QString command;        ///< Текстовая команда.
  QString text;           ///< Текст сообщения.
  quint64 name;           ///< Уникальное имя-счётчик сообещения.
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

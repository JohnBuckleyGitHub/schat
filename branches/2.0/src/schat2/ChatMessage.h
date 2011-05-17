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

#ifndef CHATMESSAGE_H_
#define CHATMESSAGE_H_

class MessageData;
class SimpleClient;

class ChatMessage
{
public:
  /// Тип сообщения.
  enum MessageType {
    UnknownType, ///< Неизвестный тип.
    UserMessage  ///< Сообщение от пользователя.
  };

  /// Состояние доставки пакета.
  enum DeliveryStatus {
    UnknownStatus = 0,      ///< Не известное состояние.
    OutgoingMessage = 1,    ///< Исходящее сообщение.
    IncomingMessage = 2,    ///< Входящее сообщение.
    Undelivered = 4,        ///< Сообщение было отправлено, подтверждение доставки не было получено.
    Delivered = 8,          ///< Сообщение было доставлено до адресата.
    Rejected = 16           ///< Сообщение было отвергнуто сервером.
  };

  /// Опции обработки сообщения.
  enum ParseOptions {
    NoParse = 0,         ///< Не обрабатывать.
    RemoveAllHtml = 1,   ///< Удалить весь html код.
    RemoveUnSafeHtml = 2 ///< Удалить небезопасный html код.
  };

  ChatMessage();
  ChatMessage(int status, const MessageData &data);
  inline DeliveryStatus status() const { return m_status; }
  inline MessageType type() const { return m_type; }
  inline QByteArray destId() const { return m_destId; }
  inline QByteArray senderId() const { return m_senderId; }
  inline QString nick() const { return m_nick; }
  inline QString text() const { return m_text; }
  inline quint64 name() const { return m_name; }
  QString messageId() const;
  void setNick(const QString &nick);
  void setText(const QString &text);

private:
  DeliveryStatus m_status;     ///< Состояние доставки сообщения.
  MessageType m_type;          ///< Тип сообщения.
  ParseOptions m_parseOptions; ///< Опции обработки сообщения.
  QByteArray m_destId;         ///< Идентификатор назначения.
  QByteArray m_senderId;       ///< Идентификатор отправителя.
  QString m_nick;              ///< Ник пользователя.
  QString m_text;              ///< Текст сообщения.
  quint64 m_name;              ///< Уникальное имя-счётчик сообещения.
};

#endif /* CHATMESSAGE_H_ */

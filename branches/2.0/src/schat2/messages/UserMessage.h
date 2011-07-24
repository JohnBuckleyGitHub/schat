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

#ifndef USERMESSAGE_H_
#define USERMESSAGE_H_

#include "messages/AbstractMessage.h"

class MessageData;
class SimpleClient;

class SCHAT_CORE_EXPORT UserMessage : public AbstractMessage
{
public:
  /// Состояние доставки пакета.
  enum DeliveryStatus {
    UnknownStatus = 0,      ///< Не известное состояние.
    OutgoingMessage = 1,    ///< Исходящее сообщение.
    IncomingMessage = 2,    ///< Входящее сообщение.
    Undelivered = 4,        ///< Сообщение было отправлено, подтверждение доставки не было получено.
    Delivered = 8,          ///< Сообщение было доставлено до адресата.
    Rejected = 16           ///< Сообщение было отвергнуто сервером.
  };

  UserMessage(int status, const MessageData &data);
  inline DeliveryStatus status() const { return m_status; }
  inline QString nick() const { return m_nick; }
  inline quint64 name() const { return m_name; }
  QString js() const;
  QString messageId() const;
  void setNick(const QString &nick);

private:
  QString setMessageState(const QString &state) const;

  DeliveryStatus m_status;     ///< Состояние доставки сообщения.
  QString m_nick;              ///< Ник пользователя.
  quint64 m_name;              ///< Уникальное имя-счётчик сообещения.
};

#endif /* USERMESSAGE_H_ */

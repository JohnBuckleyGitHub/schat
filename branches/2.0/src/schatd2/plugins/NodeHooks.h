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

#ifndef NODEHOOKS_H_
#define NODEHOOKS_H_

class MessageData;

class NodeHook
{
public:
  enum Type {
    AcceptedMessage, ///< Успешно доставленное сообщение.
    OfflineDelivery, ///< Поддержка оффлайн-доставки сообщений.
    AcceptedUser     ///< Успешно авторизированный пользователь.
  };

  virtual ~NodeHook() {}
  inline Type type() const { return m_type; }

protected:
  NodeHook(Type type)
  : m_type(type)
  {}

  Type m_type;
};


class MessageHook : public NodeHook
{
public:
  MessageHook(MessageData *data, qint64 timestamp, int status)
  : NodeHook(AcceptedMessage)
  , m_status(status)
  , m_data(data)
  , m_timestamp(timestamp)
  {}

  inline int status() const { return m_status; }
  inline MessageData *data() const { return m_data; }
  inline qint64 timestamp() const { return m_timestamp; }

protected:
  int m_status;
  MessageData *m_data;
  qint64 m_timestamp;
};

#endif /* NODEHOOKS_H_ */
